#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "../include/MemoryAllocationWrap.h"
#include "../include/StackTracerManagement.h"
#include <execinfo.h>
#include <unistd.h>
#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::lock_guard
#include <pthread.h>

void thread_test();

int main() {
    StackTracerManagement::getInstance();
    thread_test();
    return 0;
}

void thread_malloc(int size) {
    char *p1 = (char *) malloc(size);
    free(p1);
}

void thread_test() {
    std::thread threads[100];
    // spawn 10 threads:
    for (int i = 0; i < 100; ++i)
        threads[i] = std::thread(thread_malloc, (i + 1) * 10);
    for (int i = 0; i < 100; ++i) {
        threads[i].join();
    }
}