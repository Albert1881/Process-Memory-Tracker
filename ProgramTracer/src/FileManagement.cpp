//
// Created by Albert on 2021/5/22.
//

#include "../include/FileManagement.h"

int initDirectory(unsigned long long pid) {
    char path[MAX_PATH_LEN] = {0};
    sprintf(path, "/tmp/memTracer/%llu/", pid);
    clearDirectory(path);
    createDirectory(path);
}

bool removeDirectory(unsigned long long pid) {
    char path[MAX_PATH_LEN] = {0};
    sprintf(path, "/tmp/memTracer/%llu/", pid);
    printf("rmd %s\n", path);
    clearDirectory(path);
    rmdir(path);
}

int createDirectory(const char *directoryPath) {
    int dirPathLen = strlen(directoryPath);
    if (dirPathLen > MAX_PATH_LEN) {
        return -1;
    }
    char tmpDirPath[MAX_PATH_LEN] = {0};
    for (int i = 0; i < dirPathLen; ++i) {
        tmpDirPath[i] = directoryPath[i];
        if (tmpDirPath[i] == PATH_DELIMITER) {
            if (access(tmpDirPath, 0) != 0) {
                int ret = MKDIR(tmpDirPath);
                if (ret != 0) {
                    return ret;
                }
            }
        }
    }
    return 0;
}

void getfilepath(const char *path, const char *filename, char *filepath) {
    strcpy(filepath, path);
    if (filepath[strlen(path) - 1] != PATH_DELIMITER) {
        char tmp[2];
        tmp[0] = PATH_DELIMITER;
        tmp[1] = '\0';
        strcat(filepath, tmp);
    }
    strcat(filepath, filename);
}


bool clearDirectory(const char *path) {
    DIR *dir;
    struct dirent *dirinfo;
    struct stat statbuf;
    char filepath[MAX_PATH_LEN] = {0};
    lstat(path, &statbuf);

    if (S_ISREG(statbuf.st_mode)) {
        remove(path);
    } else if (S_ISDIR(statbuf.st_mode)) {
        if ((dir = opendir(path)) == NULL)
            return 1;
        while ((dirinfo = readdir(dir)) != NULL) {
            getfilepath(path, dirinfo->d_name, filepath);
            if (strcmp(dirinfo->d_name, ".") == 0 || strcmp(dirinfo->d_name, "..") == 0)
                continue;
            clearDirectory(filepath);
            rmdir(filepath);
        }
        closedir(dir);
    }
    return 0;
}

void recordInsertFile(trace_record *&record) {
    char path[MAX_PATH_LEN] = {0};

    sprintf(path, "/tmp/memTracer/%llu/%llu/%zu_%ld_%zu",
            record->pid,
            record->tid,
            reinterpret_cast<size_t>(record->address),
            reinterpret_cast<long>( record->create_time),
            record->size);
//    printf("%s\n", path);
    createDirectory(path);

    FILE *file = fopen(path, "w");
    fprintf(file, "%d\n", record->depth);
    for (int j = 0; j < record->depth; ++j) {
        char result[1024] = {0};
        char *message = record->messages[j];
        char *result_ptr = result;
        StackTracerManagement::getInstance().parseCmd(message, result_ptr);
        fprintf(file, "%s", result_ptr);
    }
    fclose(file);
}

void recordRemoveFile(trace_record *&record) {
    char path[MAX_PATH_LEN] = {0};

    sprintf(path, "/tmp/memTracer/%llu/%llu/%zu_%ld_%zu",
            record->pid,
            record->tid,
            reinterpret_cast<size_t>(record->address),
            reinterpret_cast<long>( record->create_time),
            record->size);
//    printf("rrmove %s\n", path);
    remove(path);
}