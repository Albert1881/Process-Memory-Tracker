//
// Created by Albert on 2021/5/20.
//

#include "../include/MemoryAllocationWrap.h"
#include "../include/FileManagement.h"
#include "../include/TracerSignal.h"


void *
__wrap_malloc(size_t size) {
    DEBUG("call __wrap_malloc function, size: %zu\n", size);

    void *address = __real_malloc(size);
    if (StackTracerManagement::getInstance().insert(malloc_type, address, size)) {
        DEBUG("Malloc: %zu\n", size);
    } else {
        DEBUG("Malloc error %zu\n", reinterpret_cast<size_t>(address));
    }
    return address;
}

void __wrap_free(void *ptr) {
    DEBUG("call __wrap_free function\n");
    size_t size = StackTracerManagement::getInstance().findTraceRecord(ptr)->size;
    if (StackTracerManagement::getInstance().remove(malloc_type, ptr)) {
        DEBUG("Free: %zu\n", size);
        __real_free(ptr);
    } else {
        DEBUG("Free error %zu\n", reinterpret_cast<size_t>(ptr));
    }
}

void *__wrap__Znwm(size_t size) {
    DEBUG("call __wrap__Znwm funtcion, size: %zu\n", size);
    void *address = __real__Znwm(size);
    //    DEBUG(  " %ld\n", address);
    if (StackTracerManagement::getInstance().insert(Znwm_type, address, size)) {
        DEBUG("New: %zu\n", size);
    } else {
        DEBUG("New error %zu\n", reinterpret_cast<size_t>(address));
    }
    return address;
}

void __wrap__ZdlPv(void *ptr) {
    DEBUG("call __wrap__ZdlPv function\n");
    size_t size = StackTracerManagement::getInstance().findTraceRecord(ptr)->size;

    if (StackTracerManagement::getInstance().remove(Znwm_type, ptr)) {
        DEBUG("Delete: %zu\n", size);
        __real__ZdlPv(ptr);
    } else {
        DEBUG("Delete error %zu\n", reinterpret_cast<size_t>(ptr));
    }
}

void *__array__Znwm(size_t size) {
    void *address = __real__Znwm(size);
    if (StackTracerManagement::getInstance().insert(newArr_type, address, size)) {
        DEBUG("New: %zu\n", size);
    } else {
        DEBUG("New error %zu\n", reinterpret_cast<size_t>(address));
    }
    return address;
}


void __array__ZdlPv(void *ptr) {
    size_t size = StackTracerManagement::getInstance().findTraceRecord(ptr)->size;
    if (StackTracerManagement::getInstance().remove(newArr_type, ptr)) {
        DEBUG("Delete: %zu\n", size);
        __real__ZdlPv(ptr);
    } else {
        DEBUG("Delete error %zu\n", reinterpret_cast<size_t>(ptr));
    }
}

void *operator new[](size_t size) {
    DEBUG("call new[] function\n");
    void *address = __array__Znwm(size);

    return address;
}

void operator delete[](void *ptr) {
    DEBUG("call delete[] function\n");
    __array__ZdlPv(ptr);
}

FILE *__wrap_fopen(const char *path, const char *mode) {
    DEBUG("call __wrap_fopen funtcion\n");
    FILE *fileptr = __real_fopen(path, mode);
    if (StackTracerManagement::getInstance().insert(fopen_type, reinterpret_cast<void *>(fileptr))) {
        DEBUG("fopen fd: %d\n", fileno(fileptr));
    } else {
        DEBUG("fopen error fd: %d\n", fileno(fileptr));
    }
    return fileptr;
}

FILE *__wrap_freopen(const char *filename, const char *mode, FILE *stream) {
    DEBUG("call __wrap_freopen funtcion\n");
    FILE *fileptr = __real_freopen(filename, mode, stream);
    if (fileptr != NULL) {
        trace_record *record = StackTracerManagement::getInstance().findTraceRecord(reinterpret_cast<void *>(stream));
        if (record != NULL) {
            DEBUG("remove fd: %d\n", fileno(stream));
            StackTracerManagement::getInstance().remove(record->ttype, record->address);
        } else {
            DEBUG("Don't exit fd: %d\n", fileno(stream));
        }

        if (StackTracerManagement::getInstance().insert(fopen_type, reinterpret_cast<void *>(fileptr))) {
            DEBUG("insert fd: %d\n", fileno(fileptr));
        } else {
            DEBUG("insert error fd: %d\n", fileno(fileptr));
        }
    }

    return fileptr;
}

int *__wrap_fclose(FILE *stream) {
    DEBUG("call __wrap_fclose funtcion\n");
    int *result;
    if (StackTracerManagement::getInstance().remove(fopen_type, reinterpret_cast<void *>(stream))) {
        DEBUG("fclose fd: %d\n", fileno(stream));
        result = __real_fclose(stream);
    } else {
        DEBUG("fclose error fd %d\n", fileno(stream));
    }
    return result;
}

__attribute__((constructor)) void before_main() {
    set_trace_signal();
}

__attribute__((destructor)) void after_main() {
    if (!StackTracerManagement::getInstance().isEmpty()) {
        printf("MEMORY LEAK\n");
    }
    StackTracerManagement::getInstance().getLock();
    char const *path = {PATH};
    if (strlen(path) == 0) {
        StackTracerManagement::getInstance().recordLeakerMemoryInfo(NULL);
    } else {
        createDirectory(PATH);
        StackTracerManagement::getInstance().recordLeakerMemoryInfo(path);
    }
    StackTracerManagement::getInstance().removeAll();
    removeDirectory(getpid());

}
