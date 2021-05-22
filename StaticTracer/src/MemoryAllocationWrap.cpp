//
// Created by Albert on 2021/5/20.
//

#include "../include/MemoryAllocationWrap.h"
#include "../include/FileManagement.h"
#include "../include/TracerSignal.h"

void *__wrap_malloc(size_t size) {
    fprintf(stdout, "call __wrap_malloc function, size: %zu\n", size);

    void *address = __real_malloc(size);
//    fprintf(stdout, " %ld\n", address);
    if (StackTracerManagement::getInstance().insert(address, size)) {
        fprintf(stdout, "Malloc: %zu\n", size);
    } else {
        fprintf(stdout, "Malloc error %zu\n", address);
    }
    return address;
}

void __wrap_free(void *ptr) {
    fprintf(stdout, "call __wrap_free function\n");
    size_t size = StackTracerManagement::getInstance().findTraceRecord(ptr)->size;
    if (StackTracerManagement::getInstance().remove(ptr)) {
        fprintf(stdout, "Free: %zu\n", size);
        __real_free(ptr);
    } else {
        fprintf(stdout, "Free error %zu\n", ptr);
    }
}


void *__wrap__Znwm(size_t size) {
    fprintf(stdout, "call __wrap__Znwm funtcion, size: %zu\n", size);
    void *address = __real__Znwm(size);
//    fprintf(stdout, " %ld\n", address);
    if (StackTracerManagement::getInstance().insert(address, size)) {
        fprintf(stdout, "New: %zu\n", size);
    } else {
        fprintf(stdout, "New error %zu\n", address);
    }
    return address;
}

void __wrap__ZdlPv(void *ptr) {
    fprintf(stdout, "call __wrap__ZdlPv function\n");
    size_t size = StackTracerManagement::getInstance().findTraceRecord(ptr)->size;

    if (StackTracerManagement::getInstance().remove(ptr)) {
        fprintf(stdout, "Delete: %zu\n", size);
        __real__ZdlPv(ptr);
    } else {
        fprintf(stdout, "Delete error %zu\n", ptr);
    }
}

__attribute__((constructor)) void before_main() {
    set_trace_signal();
}

__attribute__((destructor)) void after_main() {
    if (!StackTracerManagement::getInstance().isEmpty()) {
        StackTracerManagement::getInstance().recordLeakerMemoryInfo();
    }
    removeDirectory(getpid());
}
