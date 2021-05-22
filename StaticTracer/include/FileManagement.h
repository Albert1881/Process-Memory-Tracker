//
// Created by Albert on 2021/5/22.
//

#ifndef MEMORYTRACER_FILEMANAGEMENT_H
#define MEMORYTRACER_FILEMANAGEMENT_H

#include <unistd.h>
#include <sys/stat.h>
#include<stdio.h>

#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>

#include "StackTracerManagement.h"

#define MAX_PATH_LEN 256
#define PATH_DELIMITER '/'

#define MKDIR(path) mkdir(path,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)

int initDirectory(int pid);

int createDirectory(const char *directoryPath);

void getfilepath(const char *path, const char *filename, char *filepath);

bool clearDirectory(const char *path);

bool removeDirectory(int pid);

void recordInsertFile(trace_record *&record);

void recordRemoveFile(trace_record *&record);

#endif //MEMORYTRACER_FILEMANAGEMENT_H
