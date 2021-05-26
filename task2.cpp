#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <dirent.h>
#include <fstream>
#include <cstring>
#include <sys/sysinfo.h>
#include <ctime>
#include <unistd.h>

using namespace std;

typedef struct compare_result {
    vector<string> before_result;
    vector<string> after_result;
} compare_result;


typedef struct CPUPACKED         //定义一个cpu occupy的结构体
{
    char name[20];      //定义一个char类型的数组名name有20个元素
    unsigned int user; //定义一个无符号的int类型的user
    unsigned int nice; //定义一个无符号的int类型的nice
    unsigned int system;//定义一个无符号的int类型的system
    unsigned int idle; //定义一个无符号的int类型的idle
    unsigned int lowait;
    unsigned int irq;
    unsigned int softirq;
} CPU_OCCUPY;

int get_cpuoccupy(CPU_OCCUPY *cpust) {
    FILE *fd;
    char buff[256];
    CPU_OCCUPY *cpu_occupy;
    cpu_occupy = cpust;

    fd = fopen("/proc/stat", "r");
    fgets(buff, sizeof(buff), fd);

    sscanf(buff, "%s %u %u %u %u %u %u %u", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice, &cpu_occupy->system,
           &cpu_occupy->idle, &cpu_occupy->lowait, &cpu_occupy->irq, &cpu_occupy->softirq);

    fclose(fd);
    return 0;
}


float cal_cpu_occupy(CPU_OCCUPY *cpu_stat1, CPU_OCCUPY *cpu_stat2) {
    unsigned long od, nd;
    double cpu_use;

    od = (unsigned long) (cpu_stat1->user + cpu_stat1->nice + cpu_stat1->system + cpu_stat1->idle + cpu_stat1->lowait +
                          cpu_stat1->irq +
                          cpu_stat1->softirq);
    nd = (unsigned long) (cpu_stat2->user + cpu_stat2->nice + cpu_stat2->system + cpu_stat2->idle + cpu_stat2->lowait +
                          cpu_stat2->irq +
                          cpu_stat2->softirq);

    double sum = nd - od;
    double idle = cpu_stat2->user + cpu_stat2->system + cpu_stat2->nice - cpu_stat1->user - cpu_stat1->system -
                  cpu_stat1->nice;
    cpu_use = idle / sum;
    return cpu_use;
}

float get_cpu_usage() {
    CPU_OCCUPY cpu_stat1;
    CPU_OCCUPY cpu_stat2;

    get_cpuoccupy((CPU_OCCUPY *) &cpu_stat1);

    usleep(200000);

    //第二次获取cpu使用情况
    get_cpuoccupy((CPU_OCCUPY *) &cpu_stat2);
    //计算cpu使用率

    float cpu_use = cal_cpu_occupy((CPU_OCCUPY *) &cpu_stat1, (CPU_OCCUPY *) &cpu_stat2);
    return cpu_use;
}

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

vector<string> get_dir(string path) {
    vector<string> result;
    DIR *dir;
    struct dirent *ptr;
    dir = opendir(path.data());
    ptr = readdir(dir);
    while (ptr) {
        if (ptr->d_name[0] != '.')
            result.push_back(ptr->d_name);
        ptr = readdir(dir);
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

void print_file(string path) {
    ifstream inFile(path);
    string str;
    while (inFile.good()) {
        getline(inFile, str);  //该方法来自<string>
        cout << str << endl;
    }
}

void print_details(int pid) {
    vector<string> dir = get_dir("/tmp/memTracer/" + to_string(pid));
    for (int i = 0; i < dir.size(); i++) {
        vector<string> dir_2 = get_dir("/tmp/memTracer/" + to_string(pid) + "/" + dir[i]);
        for (int j = 0; j < dir_2.size(); j++) {
            string fin_dir = "/tmp/memTracer/" + to_string(pid) + "/" + dir[i] + "/" + dir_2[j];
            cout << "------------------------------------------------------\n";
            cout << "pid: " << pid << "\n";
            cout << "tid: " << dir[i] << "\n";
            cout << "address: " << dir_2[j].substr(0, dir_2[j].find('_')) << "\n";
            string time_s = dir_2[j].substr(dir_2[j].find('_') + 1, dir_2[j].rfind('_'));
            time_t time = atoll(time_s.c_str());
            cout << "time: " << ctime(&time);
            cout << "size: " << dir_2[j].substr(dir_2[j].rfind('_') + 1, dir_2[j].size()) << "\n";
            cout << "------------------------------------------------------\n";
            print_file(fin_dir);
        }

    }


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

        cout << "The process " << pid << " maps last:\n";
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
    // vector<string> result = get_dir("/proc");
    // // executeCMD("ls -l /proc/1/fd");
    // for (int i = 0; i < result.size(); i++) {
    //     cout << result[i] << " ";
    // }

    //print_file("/home/yuanz/CLionProjects/osproj2_task2/123");
    // cout << getpid();
    //  cout << "pls input the pid:\n";
    //  int input_pid = 0;
    //  cin >> input_pid;
    //  float flo = GetCpuUsageRatio(input_pid);
    //  cout << flo;
    cout << get_cpu_usage();

    return 0;
}
