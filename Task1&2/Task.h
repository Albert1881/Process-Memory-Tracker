//
// Created by gaosd on 2021/5/25.
//

#ifndef OS_PROJ2_TASK_H
#define OS_PROJ2_TASK_H
#include <string>
#include <set>
#include <mutex>
#include <vector>

#define MAX 1024
#define PATH_SIZE 128
typedef unsigned long long ull;

struct Process{
    int pid{};
    std::string name;
    ull VmSize{}; //进程虚拟地址空间的大小
    ull VmRSS{};  //所占物理内存的大小
    ull VmData{}; //所占虚拟内存的大小
    double VmRSSrate{};//所占物理内存大小比例
    Process(int pid,std::string name,ull VmSize,ull VmRSS,ull VmData,double VmRSSrate):pid(pid),name(std::move(name)),VmSize(VmSize),VmRSS(VmRSS),VmData(VmData),VmRSSrate(VmRSSrate){}
    Process()= default;
    bool operator < (const Process &p)const{
        return VmRSS > p.VmRSS;
    }
};
ull getTotalMem();
void getMemInfo(std::mutex &mt,std::set<Process>&mySet,std::set<int>&pids,ull totalMem);
void getSuspiciousPid(std::vector<int> &list,std::set<Process>&mySet);
void traverseMemInfo(std::mutex &mt,std::set<Process>&mySet);
void detectCertainPid(std::set<Process>&mySet,std::set<int>&pids);




#endif //OS_PROJ2_TASK_H
