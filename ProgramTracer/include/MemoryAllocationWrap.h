//
// Created by Albert on 2021/5/20.
//

#ifndef PROGRAMTRACER_MEMORYALLOCATIONWRAP_H
#define PROGRAMTRACER_MEMORYALLOCATIONWRAP_H

#include "tracerConfig.h"
#include <stdlib.h>
#include <execinfo.h>
#include "../include/StackTracerManagement.h"

#ifndef PATH
PATH NULL
#endif

#ifdef DEBUG_BUILD
#if (DEBUG_BUILD)
#define DEBUG(format, ...) fprintf(stderr, format, ##__VA_ARGS__)
#else
#define DEBUG(format, ...) \
    do                     \
    {                      \
    } while (0)
#endif
#else
#define DEBUG(format, ...) \
    do                     \
    {                      \
    } while (0)
#endif

extern "C" {
void *__wrap_malloc(size_t size);
void __wrap_free(void *ptr);

void *__real_malloc(size_t size);
void __real_free(void *ptr);

void *__wrap__Znwm(size_t size);
void __wrap__ZdlPv(void *ptr);

void *__array__Znwm(size_t size);
void __array__ZdlPv(void *ptr);

void *__real__Znwm(unsigned long size);
void __real__ZdlPv(void *ptr);

FILE *__wrap_fopen(const char *path, const char *mode);
FILE *__real_fopen(const char *path, const char *mode);

FILE *__wrap_freopen(const char *filename, const char *mode, FILE *stream);
FILE *__real_freopen(const char *filename, const char *mode, FILE *stream);

int *__wrap_fclose(FILE *stream);
int *__real_fclose(FILE *stream);

int backtrace(void **buffer, int size);

__attribute__((destructor)) void after_main();
__attribute__((constructor)) void before_main();


} // extern "C"



#endif //PROGRAMTRACER_MEMORYALLOCATIONWRAP_H
