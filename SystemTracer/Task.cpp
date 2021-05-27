//
// Created by gaosd on 2021/5/25.
//
#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <algorithm>
#include <cstring>
#include <fstream>
#include "Task.h"
#include <set>
//CPU
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
//Task1
ull getTotalMem(){
    FILE *fp = fopen("/proc/meminfo", "r"); /* 打开文件 */
    char buf[MAX];
    if (fp == nullptr) {
        perror("fail to open");
        return 0;
    }
    std::string totalMem;
    while (fgets(buf, MAX, fp) != nullptr) { /* 读取每一行 */
        if (buf[0] == 'M'
            && buf[1] == 'e'
            && buf[2] == 'm'
            && buf[3] == 'T'
            && buf[4] == 'o'
            && buf[5] == 't'
            && buf[6] == 'a'
            && buf[7] == 'l') {
            int i = 10;
            while (buf[i] != '\n') {
                totalMem += buf[i];
                i++;
            }
            return strtoull(totalMem.c_str(),nullptr,10);
        }
    }
}

void getMemInfo(std::mutex &mt,std::map<int,Process>&mySet,ull totalMem) {
    while (true) {
        mt.lock();
        mySet.clear();
        std::set<int>pids;
        std::string process_id, process_name, process_vmsize,process_vmrss,process_vmdata;
        DIR *dir;
        struct dirent *entry;
        FILE *fp;
        char path[PATH_SIZE];
        char buf[MAX];
        if ((dir = opendir("/proc")) == nullptr) { /* 打开/proc目录 */
            perror("fail to open dir");
            mt.unlock();
            continue;
        }
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_name[0] == '.') /* 跳过当前目录，proc目录没有父目录 */
                continue;
            /* 跳过系统信息目录，所有进程的目录全都是数字，而系统信息目录全都不是数字 */
            if ((entry->d_name[0] <= '0') || (entry->d_name[0] >= '9'))
                continue;
            /* 使用sprintf完成拼接路径，其中两个%s会由entry->d_name表示的进程ID替
            代 */
            sprintf(path, "/proc/%s/task/%s/status", entry->d_name, entry->d_name);
            fp = fopen(path, "r"); /* 打开文件 */
            if (fp == nullptr) {
                perror("fail to open");
                mt.unlock();
                return;
            }
            while (fgets(buf, MAX, fp) != nullptr) { /* 读取每一行 */
                if (buf[0] == 'N'
                    && buf[1] == 'a'
                    && buf[2] == 'm'
                    && buf[3] == 'e') {
                    int i = 6;
                    while (buf[i] != '\n') {
                        process_name += buf[i];
                        i++;
                    }
                }
                if (buf[0] == 'P'
                    && buf[1] == 'i'
                    && buf[2] == 'd') {
                    int i = 5;
                    while (buf[i] != '\n') {
                        process_id += buf[i];
                        i++;
                    }
                }
                if (buf[0] == 'V'
                    && buf[1] == 'm'
                    && buf[2] == 'S'
                    && buf[3] == 'i'
                    && buf[4] == 'z'
                    && buf[5] == 'e') {
                    int i = 8;
                    while (buf[i] != '\n') {
                        process_vmsize += buf[i];
                        i++;
                    }
                }
                if (buf[0] == 'V'
                    && buf[1] == 'm'
                    && buf[2] == 'R'
                    && buf[3] == 'S'
                    && buf[4] == 'S') {
                    int i = 7;
                    while (buf[i] != '\n') {
                        process_vmrss += buf[i];
                        i++;
                    }
                }
                if (buf[0] == 'V'
                    && buf[1] == 'm'
                    && buf[2] == 'D'
                    && buf[3] == 'a'
                    && buf[4] == 't'
                    && buf[5] == 'a') {
                    int i = 8;
                    while (buf[i] != '\n') {
                        process_vmdata += buf[i];
                        i++;
                    }
                }
            }
            int p = (int)strtol(process_id.c_str(),nullptr,10);
            pids.insert(p);
            auto iter = mySet.find(p);
            //update
            if(iter != mySet.end() && process_name == iter->second.name) {
                iter->second.lastVmSize = iter->second.VmSize;
                iter->second.VmSize = strtoull(process_vmsize.c_str(), nullptr, 10);
                iter->second.VmRSS = strtoull(process_vmrss.c_str(), nullptr, 10);
                iter->second.VmData = strtoull(process_vmdata.c_str(), nullptr, 10);
                iter->second.VmSizerate = (double) strtoull(process_vmsize.c_str(), nullptr, 10) /(double) totalMem;
            }
            //insert
            else {
                mySet.insert(std::make_pair(p,Process(p,
                                                    process_name,
                                                    0,
                                                    strtoull(process_vmsize.c_str(), nullptr, 10),
                                                    strtoull(process_vmrss.c_str(), nullptr, 10),
                                                    strtoull(process_vmdata.c_str(), nullptr, 10),
                                                    (double) strtoull(process_vmsize.c_str(), nullptr, 10) /
                                                    (double) totalMem)));
            }
            process_id = "";
            process_name = "";
            process_vmsize = "";
            process_vmrss = "";
            process_vmdata = "";
            fclose(fp); /* 关闭stattus文件 */
        }
        closedir(dir); /* 关闭目录 */
        for(auto iter = mySet.begin();iter!=mySet.end();){
            if(pids.find(iter->first)==pids.end()){
                mySet.erase(iter++);
            }else{
                ++iter;
            }
        }
        pids.clear();
        mt.unlock();
        //Sleep的精度是ms级的，需要更精确更新需要更换
        sleep(1);
    }
}

void getSuspiciousPid(std::vector<int> &list,std::map<int,Process>&mySet){
    for (const auto & iter : mySet) {
        if(iter.second.VmSizerate > 1) {
            list.push_back(iter.first);
        }
        else if(iter.second.VmSize > 100 * iter.second.VmRSS  && 5 * iter.second.lastVmSize < iter.second.VmSize){
            list.push_back(iter.first);
        }
    }
}

void traverseMemInfo(std::mutex &mt,std::map<int,Process>&mySet) {
    std::set<Process>tmp;
    mt.lock();
    for (const auto & iter : mySet) {
        tmp.insert(Process(iter.first,iter.second.name,iter.second.VmSize,iter.second.VmRSS));
    }
    mt.unlock();
    printf("\nThere are %lu datas\n",mySet.size());
    printf("name\tpid\tVmSize(KB)\tVmRSS(KB)\n");
    for (const auto & iter : tmp) {
        printf("%s\t%d\t%llu\t%llu\n",iter.name.c_str(),iter.pid,iter.VmSize,iter.VmRSS);
        //std::cout<<iter.VmRSSrate<<"\n";
    }
    tmp.clear();
}

int detectCertainPid(std::map<int,Process>&mySet) {
    std::string s_pid;
    std::vector<int>suspiciousPid;
    getSuspiciousPid(suspiciousPid,mySet);
    printf("Please enter the pid:\nThe suspicious pids are:");
    for(int p : suspiciousPid){
        printf("%d ",p);
    }
    suspiciousPid.swap(suspiciousPid);
    printf("\n");
    std::cin>>s_pid;
    int pid = (int)strtol(s_pid.c_str(), nullptr,10);
    if(pid == 0 || mySet.find(pid) == mySet.end()){
        printf("Please enter the correct pid\n");
        return -1;
    }else{
        printf("This is %d\n",pid);
        return pid;
    }
}

//Task2
std::vector<std::string>get_dir(const std::string& path){
    std::vector<std::string> result;
    DIR *dir;
    struct dirent *ptr;
    dir = opendir(path.data());
    ptr = readdir(dir);
    while (ptr) {
        if (ptr->d_name[0] != '.')
            result.emplace_back(ptr->d_name);
        ptr = readdir(dir);
    }
    return result;
}
std::vector<std::string> execute_cmd(const char *cmd) {
    std::vector<std::string> result;
    char buf_ps[1024];
    char ps[1024] = {0};
    FILE *ptr;
    std::strcpy(ps, cmd);
    if ((ptr = popen(ps, "r")) != nullptr) {
        while (fgets(buf_ps, 1024, ptr) != nullptr) {
            result.emplace_back(buf_ps);
        }
        pclose(ptr);
        ptr = nullptr;
    } else {
        printf("popen %s error\n", ps);
    }
    return result;
}

std::vector<std::string> get_fd(int &pid) {
    std::string cmd = "ls -l /proc/" + std::to_string(pid) + "/fd";
    std::vector<std::string> fd_result = execute_cmd(cmd.data());
    return fd_result;
}

std::vector<std::string> get_maps(int &pid) {
    std::string cmd = "cat /proc/" + std::to_string(pid) + "/maps";
    std::vector<std::string> maps_result = execute_cmd(cmd.data());
    return maps_result;
}

compare_result compare_vector_string(std::vector<std::string> before, std::vector<std::string> after) {
    std::vector<std::string> before_result;
    std::vector<std::string> after_result;
    for (auto & i : before) {
        if (find(after.begin(), after.end(), i) == after.end()) {
            before_result.push_back(i);
        }
    }
    for (auto & i : after) {
        if (find(before.begin(), before.end(), i) == before.end()) {
            after_result.push_back(i);
        }
    }
    return {before_result, after_result};
}

void print_compare_result(std::mutex &mt,int &pid,bool &flag) {
    std::vector<std::string> before_fd, after_fd, before_maps, after_maps;
    before_fd = get_fd(pid);
    before_maps = get_maps(pid);
    sleep(1);
    while (flag) {
        mt.lock();
        after_fd = get_fd(pid);
        after_maps = get_maps(pid);

        time_t now = time(nullptr);
        char *dt = ctime(&now);
        std::cout << "time:" << dt << "\n";;

        compare_result result_fd = compare_vector_string(before_fd, after_fd);
        compare_result result_maps = compare_vector_string(before_maps, after_maps);

        std::cout << "The process " << pid << " maps now lacks:\n";
        std::cout << "--------------------------------------\n";

        for (auto & i : result_maps.before_result) {
            std::cout << i;
        }
        std::cout << "\n";
        std::cout << "Now the process " << pid << " maps has increased:\n";

        std::cout << "--------------------------------------\n";

        for (auto & i : result_maps.after_result) {
            std::cout << i;
        }

        std::cout << "\n";

        std::cout << "The process " << pid << " fd now lacks:\n";
        std::cout << "--------------------------------------\n";

        for (auto & i : result_fd.before_result) {
            std::cout << i;
        }
        std::cout << "\n";
        std::cout << "Now the process " << pid << " fd has increased:\n";

        std::cout << "--------------------------------------\n";

        for (auto & i : result_fd.after_result) {
            std::cout << i;
        }
        std::cout << "\n";;
        before_fd = after_fd;
        before_maps = after_maps;
        mt.unlock();
        sleep(10);
    }
}

void print_detect_static(int &pid){
    std::vector<std::string> dir = get_dir("/tmp/memTracer/" + std::to_string(pid));
    for (auto & i : dir) {
        std::vector<std::string> dir_2 = get_dir("/tmp/memTracer/" + std::to_string(pid) + "/" + i);
        for (auto & j : dir_2) {
            std::string fin_dir = "/tmp/memTracer/" + std::to_string(pid) + "/" + i + "/" + j;
            std::cout << "------------------------------------------------------\n";
            std::cout << "pid: " << pid << "\n";
            std::cout << "tid: " << i << "\n";
            std::cout << "address: " << j.substr(0, j.find('_')) << "\n";
            std::string time_s = j.substr(j.find('_') + 1, j.rfind('_'));
            time_t time = atoll(time_s.c_str());
            std::cout << "time: " << ctime(&time);
            std::cout << "size: " << j.substr(j.rfind('_') + 1, j.size()) << "\n";
            std::cout << "------------------------------------------------------\n";
//            print_file(fin_dir);
            std::ifstream inFile(fin_dir);
            std::string str;
            while(inFile.good()){
                getline(inFile,str);
                std::cout<<str<<"\n";
            }
        }

    }

}

