//
// Created by Albert on 2021/5/20.
//

#include "../include/MemoryAllocationWrap.h"
#include "../include/FileManagement.h"
#include "../include/TracerSignal.h"

void *__wrap_malloc(size_t size) {
    // printf("call __wrap_malloc function, size: %zu\n", size);

    void *address = __real_malloc(size);
//    // printf(  " %ld\n", address);
    if (StackTracerManagement::getInstance().insert(address, size)) {
        // printf("Malloc: %zu\n", size);
    } else {
        // printf("Malloc error %zu\n", reinterpret_cast<size_t>(address));
    }
    return address;
}

void __wrap_free(void *ptr) {
    // printf("call __wrap_free function\n");
    size_t size = StackTracerManagement::getInstance().findTraceRecord(ptr)->size;
    if (StackTracerManagement::getInstance().remove(ptr)) {
        // printf("Free: %zu\n", size);
        __real_free(ptr);
    } else {
        // printf("Free error %zu\n", reinterpret_cast<size_t>(ptr));
    }
}


void *__wrap__Znwm(size_t size) {
    // printf("call __wrap__Znwm funtcion, size: %zu\n", size);
    void *address = __real__Znwm(size);
//    // printf(  " %ld\n", address);
    if (StackTracerManagement::getInstance().insert(address, size)) {
        // printf("New: %zu\n", size);
    } else {
        // printf("New error %zu\n", reinterpret_cast<size_t>(address));
    }
    return address;
}

void __wrap__ZdlPv(void *ptr) {
    // printf("call __wrap__ZdlPv function\n");
    size_t size = StackTracerManagement::getInstance().findTraceRecord(ptr)->size;

    if (StackTracerManagement::getInstance().remove(ptr)) {
        // printf("Delete: %zu\n", size);
        __real__ZdlPv(ptr);
    } else {
        // printf("Delete error %zu\n", reinterpret_cast<size_t>(ptr));
    }
}

__attribute__((constructor)) void before_main() {
    set_trace_signal();
}

__attribute__((destructor)) void after_main() {
    if (!StackTracerManagement::getInstance().isEmpty()) {
        char const *path = {"./leakInfo"};
        StackTracerManagement::getInstance().recordLeakerMemoryInfo(path);
    }

    removeDirectory(getpid());
}
