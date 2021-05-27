#include <stdio.h>
#include <stdlib.h>
#include <iostream>
//#include "../include/MemoryAllocationWrap.h"
//#include "../include/StackTracerManagement.h"
#include <execinfo.h>
#include <unistd.h>
#include <thread> // std::thread
#include <mutex>  // std::mutex, std::lock_guard
#include <pthread.h>

void malloc_test();

void new_test();

void new_array_test();

void fopen_test();

void freopen_test();

void thread_test();


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
    malloc_test();
    return 0;
}

void malloc_test() {
    fprintf(stdout, "===== malloc_test start =====\n");
    char *str_noleak;
    str_noleak = (char *) malloc(15);
    strcpy(str_noleak, "It's malloc_test. \nThe str didn't leak.");
    printf("String = %s,  Address = %u\n", str_noleak, reinterpret_cast<size_t>(str_noleak));

    char *str_leak;
    str_leak = (char *) malloc(15);
    strcpy(str_leak, "It's malloc_test. \nThe str did leak.");
    printf("String = %s,  Address = %u\n", str_leak, reinterpret_cast<size_t>(str_leak));
    free(str_noleak);
    fprintf(stdout, "===== malloc_test finish =====\n");

}

void new_test() {
    fprintf(stdout, "===== new_test start =====\n");
    father *p = new son;
    delete p;
    fprintf(stdout, "===== new_test finish =====\n");
}

void new_array_test() {
    fprintf(stdout, "===== new_array_test start =====\n");
    A *p = new A[5];
    delete[] p;

    int *int_ptr = new int[10];
    for (int i = 0; i < 10; ++i) {
        int_ptr[i] = i;
    }
    delete[] (int_ptr);
    fprintf(stdout, "===== new_array_test finish =====\n");
}

void fopen_test() {
    fprintf(stdout, "===== fopen_test start =====\n");

    FILE *file_noleak = fopen("fopen_noleak", "w");

    if (file_noleak == NULL) {
        printf("open file fopen_noleak failed!\n");
    } else {
        printf("open file fopen_noleak succeed!\n");
        fprintf(file_noleak, "It's fopen_test. \nThe file didn't leak.");
    }
    FILE *file_leak = fopen("fopen_leak", "w");
    if (file_noleak == NULL) {
        printf("open file fopen_leak failed!\n");
    } else {
        printf("open file fopen_leak succeed!\n");
        fprintf(file_noleak, "It's fopen_test. \nThe file did leak.");
    }
    fclose(file_noleak);
    fprintf(stdout, "===== fopen_test finish =====\n");
}

void freopen_test() {
    fprintf(stdout, "===== freopen_test start =====\n");
    FILE *fp;
    printf("This text is in stdout\n");
    fp = freopen("freopen_test", "w+", stdout);
    printf("This text is in freopen_test\n");
    fclose(fp);
    fprintf(stdout, "===== freopen_test finish =====\n");
}


void thread_malloc(int size) {
    char *ptr = (char *) malloc(size);
    free(ptr);
}

void thread_test() {
    fprintf(stdout, "===== thread_test start =====\n");

    std::thread threads[10];
    for (int i = 0; i < 10; ++i) {
        threads[i] = std::thread(thread_malloc, (i + 1) * 10);
    }
    for (auto &th : threads) {
        th.join();
    }
    fprintf(stdout, "===== thread_test finish =====\n");
}

