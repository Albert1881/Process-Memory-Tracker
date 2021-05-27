//
// Created by Albert on 2021/5/21.
//

#include "../include/TracerSignal.h"
#include "../include/StackTracerManagement.h"

void set_trace_signal() {
    signal(SIGSEGV, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGFPE, signal_handler);
    signal(SIGILL, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGABRT, signal_handler);
}


void signal_handler(int signum) {

    switch (signum) {
        case SIGABRT:
            printf("Caught SIGABRT: Abnormal termination of the program, such as a call to abort.\n");
            break;
        case SIGFPE:
            printf("Caught SIGFPE: An erroneous arithmetic operation, such as a divide by zero or an operation resulting in overflow.\n");
            break;
        case SIGILL:
            printf("Caught SIGILL: Detection of an illegal instruction.\n");
            break;
        case SIGINT:
            printf("Caught SIGINT: Receipt of an interactive attention signal, (usually ctrl+c).\n");
            break;
        case SIGSEGV:
            printf("Caught SIGSEGV: Segmentation fault.\n");
            break;
        case SIGTERM:
            printf("Caught SIGTERM: A termination request sent to the program.\n");
            break;

        default:
            printf("Caught signal %d", signum);
            break;

    }
    StackTracerManagement::getInstance().getLock();
    StackTracerManagement::getInstance().insert_unlock(exception_type, NULL, 0);
    exit(signum);
}

