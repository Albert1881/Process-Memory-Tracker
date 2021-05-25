//
// Created by gaosd on 2021/5/25.
//
#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include "Task.h"

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

void getMemInfo(std::mutex &mt,std::set<Process>&mySet,std::set<int>&pids,ull totalMem) {
    while (true) {
        mt.lock();
        mySet.clear();
        pids.clear();
        std::string process_id, process_name, process_vmsize,process_vmrss,process_vmdata;
        DIR *dir;
        struct dirent *entry;
        FILE *fp;
        char path[PATH_SIZE];
        char buf[MAX];
        if ((dir = opendir("/proc")) == nullptr) { /* 打开/proc目录 */
            perror("fail to open dir");
            return;
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
            mySet.insert(Process(strtol(process_id.c_str(),nullptr,10),
                                 process_name,
                                 strtoull(process_vmsize.c_str(),nullptr,10),
                                 strtoull(process_vmrss.c_str(),nullptr,10),
                                 strtoull(process_vmdata.c_str(),nullptr,10),
                                 strtoull(process_vmsize.c_str(),nullptr,10)/(double)totalMem));
            pids.insert(strtol(process_id.c_str(), nullptr,10));
            process_id = "";
            process_name = "";
            process_vmsize = "";
            process_vmrss = "";
            process_vmdata = "";
            fclose(fp); /* 关闭stattus文件 */
        }
        closedir(dir); /* 关闭目录 */
        mt.unlock();
        //Sleep的精度是ms级的，需要更精确更新需要更换
        sleep(1);
    }
}

void getSuspiciousPid(std::vector<int> &list,std::set<Process>&mySet){
    for (const auto & iter : mySet) {
        if(iter.VmRSSrate >= 1) {
            list.push_back(iter.pid);
        }
    }
}

void traverseMemInfo(std::mutex &mt,std::set<Process>&mySet) {
    mt.lock();
    printf("\nThere are %lu datas\n",mySet.size());
    printf("name\tpid\tVmRSS(KB)\tVmData(KB)\n");
    for (const auto & iter : mySet) {
        printf("%s\t%d\t%llu\t%llu\t%llu\n",iter.name.c_str(),iter.pid,iter.VmSize,iter.VmRSS,iter.VmData);
        //std::cout<<iter.VmRSSrate<<"\n";
    }
    mt.unlock();
}

void detectCertainPid(std::set<Process>&mySet,std::set<int>&pids) {
    std::string s_pid;
    std::vector<int>suspiciousPid;
    getSuspiciousPid(suspiciousPid,mySet);
    printf("Please enter the pid:\nThe suspicious pids are:");
    for(int p : suspiciousPid){
        printf("%d ",p);
    }
    printf("\n");
    std::cin>>s_pid;
    int pid = strtol(s_pid.c_str(), nullptr,10);
    if(pid == 0 || pids.find(pid) == pids.end()){
        printf("Please enter the correct pid\n");
        return;
    }else{
        printf("This is %d\n",pid);
        
    }
}



