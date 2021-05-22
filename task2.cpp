#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <dirent.h>


using namespace std;

typedef struct compare_result {
    vector<string> before_result;
    vector<string> after_result;
} compare_result;

vector<string> execute_cmd(const char *cmd) {
    vector<string> result;
    char buf_ps[1024];
    char ps[1024] = {0};
    FILE *ptr;
    strcpy(ps, cmd);
    if ((ptr = popen(ps, "r")) != NULL) {
        while (fgets(buf_ps, 1024, ptr) != NULL) {
            result.push_back(buf_ps);
        }
        pclose(ptr);
        ptr = NULL;
    } else {
        printf("popen %s error\n", ps);
    }
    return result;
}

vector<string> get_all_pid() {
    vector<string> result;
    DIR *dir;
    struct dirent *ptr;
    dir = opendir("/proc");
    ptr = readdir(dir);
    while (ptr) {
        if (ptr->d_name[0] > '0' && ptr->d_name[0] <= '9') {
            result.push_back(ptr->d_name);
        }
        ptr = readdir(dir);
    }
    return result;
}


vector<string> get_fd(int pid) {
    string cmd = "ls -l /proc/" + to_string(pid) + "/fd";
    vector<string> fd_result = execute_cmd(cmd.data());
    return fd_result;
}

vector<string> get_maps(int pid) {
    string cmd = "cat /proc/" + to_string(pid) + "/maps";
    vector<string> maps_result = execute_cmd(cmd.data());
    return maps_result;
}

compare_result compare_vector_string(vector<string> before, vector<string> after) {
    vector<string> before_result;
    vector<string> after_result;
    for (int i = 0; i < before.size(); i++) {
        if (find(after.begin(), after.end(), before[i]) == after.end()) {
            before_result.push_back(before[i]);
        }
    }
    for (int i = 0; i < after.size(); i++) {
        if (find(before.begin(), before.end(), after[i]) == before.end()) {
            after_result.push_back(after[i]);
        }
    }
    return {before_result, after_result};
}

void print_compare_result(int pid) {

    vector<string> before_fd, after_fd, before_maps, after_maps;
    before_fd = get_fd(pid);
    before_maps = get_maps(pid);
    while (1) {
        sleep(10);
        after_fd = get_fd(pid);
        after_maps = get_maps(pid);

        time_t now = time(0);
        char *dt = ctime(&now);
        cout << "time:" << dt << "\n";;

        compare_result result_fd = compare_vector_string(before_fd, after_fd);
        compare_result result_maps = compare_vector_string(before_maps, after_maps);

        cout << "The process " << pid << " maps now lacks:\n";
        cout << "--------------------------------------\n";

        for (int i = 0; i < result_maps.before_result.size(); i++) {
            cout << result_maps.before_result[i];
        }
        cout << "\n";
        cout << "Now the process " << pid << " maps has increased:\n";

        cout << "--------------------------------------\n";

        for (int i = 0; i < result_maps.after_result.size(); i++) {
            cout << result_maps.after_result[i];
        }

        cout << "\n";

        cout << "The process " << pid << " fd now lacks:\n";
        cout << "--------------------------------------\n";

        for (int i = 0; i < result_fd.before_result.size(); i++) {
            cout << result_fd.before_result[i];
        }
        cout << "\n";
        cout << "Now the process " << pid << " fd has increased:\n";

        cout << "--------------------------------------\n";

        for (int i = 0; i < result_fd.after_result.size(); i++) {
            cout << result_fd.after_result[i];
        }
        cout << "\n";
        cout << endl;
        before_fd = after_fd;
        before_maps = after_maps;
    }
}


int main() {
    vector<string> result = get_all_pid();
    // executeCMD("ls -l /proc/1/fd");
    for (int i = 0; i < result.size(); i++) {
        cout << result[i] << " ";
    }


    // cout << "pls input the pid:\n";
    // int input_pid = 0;
    // cin >> input_pid;
    // print_compare_result(input_pid);


    return 0;
}
