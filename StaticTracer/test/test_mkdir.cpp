//
// Created by Albert on 2021/5/22.
//

#include "../include/FileManagement.h"
#include <unistd.h>

int main() {
//    initDirectory(getpid());
    remove("/tmp/memTracer/123/");
    return 0;
}