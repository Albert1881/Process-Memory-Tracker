//
// Created by Albert on 2021/5/21.
//

#ifndef MEMORYTRACER_TRACERSIGNAL_H
#define MEMORYTRACER_TRACERSIGNAL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <err.h>
#include <execinfo.h>


void set_trace_signal();

void signal_handler(int signum);

#endif //MEMORYTRACER_TRACERSIGNAL_H
