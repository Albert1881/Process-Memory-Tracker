//
// Created by Albert on 2021/5/20.
//

#ifndef MEMORYTRACER_MEMORYALLOCATIONWRAP_H
#define MEMORYTRACER_MEMORYALLOCATIONWRAP_H

#include "tracerConfig.h"
#include <stdlib.h>
#include <execinfo.h>
#include "StackTracerManagement.h"


extern "C" {
void *__wrap_malloc(size_t size);
void __wrap_free(void *ptr);

void *__real_malloc(size_t size);
void __real_free(void *ptr);

void *__wrap__Znwm(size_t size);
void __wrap__ZdlPv(void *ptr);

void *__real__Znwm(unsigned long size);
void __real__ZdlPv(void *ptr);

int backtrace(void **buffer, int size);

struct trace_record allocate();

__attribute__((destructor)) void after_main();
__attribute__((constructor)) void before_main();


} // extern "C"



#endif //MEMORYTRACER_MEMORYALLOCATIONWRAP_H