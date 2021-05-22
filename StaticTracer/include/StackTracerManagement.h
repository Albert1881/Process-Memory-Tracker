//
// Created by Albert on 2021/5/20.
//

#ifndef MEMORYTRACER_STACKTRACERMANAGEMENT_H
#define MEMORYTRACER_STACKTRACERMANAGEMENT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <thread>
#include <mutex>
#include <sstream>


#include "MemoryAllocationWrap.h"
#include "TracerSignal.h"

#define MAX_STACK_FRAMES 64
#define MAX_PROG_NAME_LENGTH 64
#define MAX_PROG_ADDR_LENGTH 64


struct trace_record {
    int pid;
    unsigned long long tid;
    void *address;
    size_t size;
    int depth;
    char **messages;
    time_t create_time;
    trace_record *next;
};

class StackTracerManagement {
public:
    static StackTracerManagement &getInstance() {
        pthread_once(&_init, &StackTracerManagement::init);
        return *_instance;
    }

    bool insert(void *ptr, size_t size);

    bool remove(void *ptr);

    void removeAll(void);

    bool isEmpty(void);

    trace_record *findTraceRecord(void *ptr);

    void recordLeakerMemoryInfo(void);

    void parseCmd(char const *message, char *&result);

private:
    int total_size = 0;

    trace_record *stack_trace_map[MAX_STACK_FRAMES];

    static pthread_once_t _init;
    static StackTracerManagement *_instance;
    std::mutex stack_trace_mutex;


private:

    StackTracerManagement(void) {
        for (int i = 0; i < MAX_STACK_FRAMES; ++i) {
            stack_trace_map[i] = NULL;
        }
    }

    ~StackTracerManagement();

    static void init();

    void setAddrBacktrace(trace_record *&record, void *ptr, size_t size);

    void releaseAddrBacktrace(trace_record *&record);

    unsigned long hashFunction(void *ptr) {
        if (ptr != NULL) {
            return (reinterpret_cast<unsigned long >(ptr) % (MAX_STACK_FRAMES - 1));
        } else {
            return (MAX_STACK_FRAMES - 1);
        }
    }

    void parseProgName(char const *message, char *&prog_name);

    void parseProgAddr(char const *message, char *&prog_addr);


    bool addr2line(char const *const program_name, void const *const address, char *result);

};


#endif //MEMORYTRACER_STACKTRACERMANAGEMENT_H
