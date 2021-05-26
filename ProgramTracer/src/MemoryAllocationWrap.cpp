//
// Created by Albert on 2021/5/20.
//

#include "../include/MemoryAllocationWrap.h"
#include "../include/FileManagement.h"
#include "../include/TracerSignal.h"

#ifndef PATH
PATH NULL
#endif

#ifdef DEBUG_BUILD
#if (DEBUG_BUILD)
#  define DEBUG(format, ...) fprintf(stderr, format, ##__VA_ARGS__)
#else
#  define DEBUG(format, ...) do {} while (0)
#endif
#else
#  define DEBUG(format, ...) do {} while (0)
#endif

void *__wrap_malloc(size_t size) {
    DEBUG("call __wrap_malloc function, size: %zu\n", size);

    void *address = __real_malloc(size);
    if (StackTracerManagement::getInstance().insert(address, size)) {
        DEBUG("Malloc: %zu\n", size);
    } else {
        DEBUG("Malloc error %zu\n", reinterpret_cast<size_t>(address));
    }
    return address;
}

void __wrap_free(void *ptr) {
    DEBUG("call __wrap_free function\n");
    size_t size = StackTracerManagement::getInstance().findTraceRecord(ptr)->size;
    if (StackTracerManagement::getInstance().remove(ptr)) {
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
    if (StackTracerManagement::getInstance().insert(address, size)) {
        DEBUG("New: %zu\n", size);
    } else {
        DEBUG("New error %zu\n", reinterpret_cast<size_t>(address));
    }
    return address;
}

void __wrap__ZdlPv(void *ptr) {
    DEBUG("call __wrap__ZdlPv function\n");
    size_t size = StackTracerManagement::getInstance().findTraceRecord(ptr)->size;

    if (StackTracerManagement::getInstance().remove(ptr)) {
        DEBUG("Delete: %zu\n", size);
        __real__ZdlPv(ptr);
    } else {
        DEBUG("Delete error %zu\n", reinterpret_cast<size_t>(ptr));
    }
}

__attribute__((constructor)) void before_main() {
    set_trace_signal();
}

__attribute__((destructor)) void after_main() {
    if (!StackTracerManagement::getInstance().isEmpty()) {
        char const *path = {PATH};
        if (strlen(path) == 0) {
            StackTracerManagement::getInstance().recordLeakerMemoryInfo(NULL);
        } else {
            createDirectory(PATH);
            StackTracerManagement::getInstance().recordLeakerMemoryInfo(path);
        }

    }

    removeDirectory(getpid());
}
