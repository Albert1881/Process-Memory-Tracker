//
// Created by Albert on 2021/5/20.
//
#include "../include/StackTracerManagement.h"
#include "../include/FileManagement.h"


char stackTrace_instance[sizeof(StackTracerManagement)];

pthread_once_t StackTracerManagement::_init = PTHREAD_ONCE_INIT;

StackTracerManagement *StackTracerManagement::_instance = NULL;

StackTracerManagement::~StackTracerManagement() {
    removeAll();
    if (!isEmpty()) {
        recordLeakerMemoryInfo();
    }
    removeDirectory(getpid());
};

void StackTracerManagement::init() {
    _instance = reinterpret_cast<StackTracerManagement *>(&stackTrace_instance);
    new(_instance)StackTracerManagement();
}


void StackTracerManagement::setAddrBacktrace(trace_record *&record, void *ptr, size_t size) {
    char **messages = (char **) NULL;
    void *traces[MAX_STACK_FRAMES];
    int trace_size = backtrace(traces, MAX_STACK_FRAMES);
    total_size += size;
    if (trace_size) {
        messages = backtrace_symbols(traces, trace_size);
    }
    record = reinterpret_cast<trace_record *>(__real__Znwm(sizeof(trace_record)));
    time(&(record->create_time));
    record->pid = reinterpret_cast<int > (getpid());

    std::ostringstream oss;
    oss << std::this_thread::get_id();
    std::string stid = oss.str();
    record->tid = std::stoull(stid);
    record->address = ptr;
    record->size = size;
    record->next = NULL;
    record->messages = messages;
    record->depth = trace_size;
    recordInsertFile(record);
}

void StackTracerManagement::releaseAddrBacktrace(trace_record *&record) {
    recordRemoveFile(record);
    total_size -= record->size;
    if (record->messages) {
        __real_free(record->messages);
    }
    __real__ZdlPv(record);
}


bool StackTracerManagement::insert(void *ptr, size_t size) {
    const std::lock_guard <std::mutex> lock(stack_trace_mutex);
    unsigned long hashValue = hashFunction(ptr);

    trace_record *prev = NULL;
    trace_record *entry = stack_trace_map[hashValue];

    while (entry != NULL && (ptr == NULL || entry->address != ptr)) {
        prev = entry;
        entry = entry->next;
    }
    if (entry == NULL) {
        setAddrBacktrace(entry, ptr, size);
        if (prev == NULL) {
            // insert as first bucket
            stack_trace_map[hashValue] = entry;
        } else {
            prev->next = entry;
        }
    } else {
        // just update the value
//        entry->setValue(value);
        return false;
    }
    return true;
}

bool StackTracerManagement::remove(void *ptr) {
    const std::lock_guard <std::mutex> lock(stack_trace_mutex);
    unsigned long hashValue = hashFunction(ptr);
    trace_record *prev = NULL;
    trace_record *entry = stack_trace_map[hashValue];
    while (entry != NULL && (ptr == NULL || entry->address != ptr)) {
        prev = entry;
        entry = entry->next;
    }
    if (entry == NULL) {
        // key not found
        return false;
    } else {
        if (prev == NULL) {
            // remove first bucket of the list
            stack_trace_map[hashValue] = entry->next;
        } else {
            prev->next = entry->next;
        }
        releaseAddrBacktrace(entry);
    }
    return true;
}

void StackTracerManagement::removeAll(void) {
    const std::lock_guard <std::mutex> lock(stack_trace_mutex);

    for (int i = 0; i < MAX_STACK_FRAMES; ++i) {
        trace_record *prev = NULL;
        trace_record *entry = stack_trace_map[i];
        while (entry != NULL) {
            prev = entry;
            entry = entry->next;
            releaseAddrBacktrace(entry);
        }
    }
}

bool StackTracerManagement::isEmpty(void) {
    const std::lock_guard <std::mutex> lock(stack_trace_mutex);

    for (int i = 0; i < MAX_STACK_FRAMES; ++i) {
        trace_record *entry = stack_trace_map[i];
        if (entry != NULL) {
            return false;
        }
    }
    return true;
}

trace_record *StackTracerManagement::findTraceRecord(void *ptr) {
    const std::lock_guard <std::mutex> lock(stack_trace_mutex);

    unsigned long hashValue = hashFunction(ptr);
    trace_record *entry = stack_trace_map[hashValue];

    while (entry != NULL) {
        if (entry->address == ptr) {
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

void StackTracerManagement::parseProgName(char const *message, char *&prog_name) {
    char fmt[32];
    sprintf(fmt, "%%%d[^(]", MAX_PROG_NAME_LENGTH);
    sscanf(message, fmt, prog_name);
    prog_name = strrchr(prog_name, '/');
    prog_name += 1;
//    printf("%s\n", program_name + 1);
}

void StackTracerManagement::parseProgAddr(char const *message, char *&prog_addr) {
    char fmt[32];
    sprintf(fmt, "%%*[^[[][%%%d[^]]]", MAX_PROG_ADDR_LENGTH);
    sscanf(message, fmt, prog_addr);
}

bool StackTracerManagement::addr2line(char const *const program_name, void const *const address, char *result) {
    char addr2line_cmd[512] = {0};
    char buff[1024] = {0};
    /* have addr2line map the address to the relent line in the code */

    sprintf(addr2line_cmd, "addr2line -f -p -e %.256s %s", program_name, (char *) (address));
//    system(addr2line_cmd);
    FILE *out = popen(addr2line_cmd, "r");
    if (out != NULL) {
        while (fgets(buff, 1024, out) != NULL) {
            strcat(result, buff);
            if (strlen(result) > 1024) {
                break;
            }
        }
        pclose(out);
        out = NULL;
        if (strlen(result) == 0) {
            return false;
        } else {
            return true;
        }

    } else {
        printf("popen %s error\n", addr2line_cmd);
        return false;
    }
}

void StackTracerManagement::parseCmd(char const *message, char *&result) {

    char prog_name[MAX_PROG_NAME_LENGTH];
    char prog_addr[MAX_PROG_ADDR_LENGTH];
    char *prog_name_ptr = prog_name;
    char *prog_addr_ptr = prog_addr;
    parseProgName(message, prog_name_ptr);
    parseProgAddr(message, prog_addr_ptr);

    if (!addr2line(prog_name_ptr, prog_addr_ptr, result)) {
        sprintf(result, "Can't parse message: %s\n", message);
    }
}

void StackTracerManagement::recordLeakerMemoryInfo(void) {
    const std::lock_guard <std::mutex> lock(stack_trace_mutex);
    for (int i = 0; i < MAX_STACK_FRAMES; ++i) {
        trace_record *prev = NULL;
        trace_record *entry = stack_trace_map[i];
        while (entry != NULL) {
            for (int j = 0; j < entry->depth; ++j) {
                char *message = entry->messages[j];
                char result[1024] = {0};
                char *result_ptr = result;
                parseCmd(message, result_ptr);
                printf("%s", result);
            }
            prev = entry;
            entry = entry->next;
        }
    }
}