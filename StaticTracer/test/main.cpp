#include <stdio.h>
#include <stdlib.h>
#include <iostream>
//#include "../include/MemoryAllocationWrap.h"
//#include "../include/StackTracerManagement.h"
#include <execinfo.h>
#include <unistd.h>
#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::lock_guard
#include <pthread.h>
// Blog: https://blog.csdn.net/fengbingchun/article/details/82947673



void thread_test();

void test1();

void test2();

void test3();

void test4();

void test5();

void print_stack_frames();

class father {
    int *p1;
public:
    father() { p1 = new int; }

    ~father() { delete p1; }
};

class son : public father {
    int *p2;
public:
    son() { p2 = new int; }

    ~son() { delete p2; }
};

class A {
    int *p1;
public:
    A() { p1 = new int; }

    ~A() { delete p1; }
};

int main() {
//    printf("MainThreadID: %ld\n",std::this_thread::get_id());
//    thread_test();
    std::hash < std::thread::id > {}(std::this_thread::get_id());
//    while (true) {
//
//    }
//    test1();
    thread_test();
    return 0;
}


void thread_malloc(int size) {
    std::thread::id tid = std::this_thread::get_id();

//    printf("ThreadID: %ld\n", tid);
    char *p1 = (char *) malloc(size);
//    free(p1);
}

void thread_test() {
    std::thread threads[10];
    // spawn 10 threads:
    for (int i = 0; i < 10; ++i)
        threads[i] = std::thread(thread_malloc, (i + 1) * 10);

    for (auto &th : threads) th.join();
}


void test1() {
    fprintf(stdout, "===== test1 start =====\n");

    char *p1 = (char *) malloc(4);

    int *p2 = new int;
    free(p1);
//    delete p2;
    fprintf(stdout, "===== test1 finish =====\n");
}

void test2() {
    fprintf(stdout, "===== test2 start =====\n");
    father *p = new son;
    delete p;
    fprintf(stdout, "===== test2 finish =====\n");
}

void test3() {
    fprintf(stdout, "===== test3 start =====\n");
    A *p = new A[5];
    delete[] p;
    fprintf(stdout, "===== test3 finish =====\n");
}

void test4() {
    test5();
}

void test5() {
    int *p1 = new int(4);
    int *p2 = new int(5);
    delete p1;
    printf("hello\n");
    print_stack_frames();
}

void print_stack_frames() {
    int j, nptrs;
#define SIZE 100
    void *buffer[100];
    char **strings;

    nptrs = backtrace(buffer, SIZE);
    printf("backtrace() returned %d addresses\n", nptrs);

    /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
       would produce similar output to the following: */

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    for (j = 0; j < nptrs; j++)
        printf("%s\n", strings[j]);

    free(strings);

    printf("maps\n");
    char buff[64] = {0x00};
    sprintf(buff, "cat /proc/%d/maps", getpid());
    system((const char *) buff);
}


