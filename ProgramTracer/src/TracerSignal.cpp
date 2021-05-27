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
        case SIGSEGV:
            printf("Caught SIGSEGV: Segmentation Fault\n");
            break;
        case SIGINT:
            printf("Caught SIGINT: Interactive attention signal, (usually ctrl+c)\n");
            break;
        case SIGFPE:
            printf("Caught SIGFPE: Arithmetic Exception\n\n");
            break;
        case SIGILL:
            printf("Caught SIGILL: Illegal Instruction\n");
            break;
        case SIGTERM:
            printf("Caught SIGTERM: a termination request was sent to the program\n");
            break;
        case SIGABRT:
            printf("Caught SIGABRT: usually caused by an abort() or assert()\n");
            break;
        default:
            printf("Caught signal %d", signum);
            break;

    }
    StackTracerManagement::getInstance().getLock();
    StackTracerManagement::getInstance().insert_unlock(exception_type, NULL, 0);
    exit(signum);
}

