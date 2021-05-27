//
// Created by gaosd on 2021/5/25.
//

#ifndef OS_PROJ2_TASK_H
#define OS_PROJ2_TASK_H
#include <string>
#include <map>
#include <mutex>
#include <vector>

#define MAX 1024
#define PATH_SIZE 128
typedef unsigned long long ull;

//CPU
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
int get_cpuoccupy(CPU_OCCUPY *cpust);
float cal_cpu_occupy(CPU_OCCUPY *cpu_stat1, CPU_OCCUPY *cpu_stat2);
float get_cpu_usage();
//Task1
struct Process{
    int pid{};
    std::string name;
    ull lastVmSize{};
    ull VmSize{}; //进程正在占用的内存
    ull VmRSS{};  //正使用物理内存的大小
    ull VmData{}; //进程数据段的大小
    double VmSizerate{};//所占物理内存大小比例
    Process(int pid,std::string name,ull lastVmSize,ull VmSize,ull VmRSS,ull VmData,double VmSizerate):pid(pid),name(std::move(name)),lastVmSize(lastVmSize),VmSize(VmSize),VmRSS(VmRSS),VmData(VmData),VmSizerate(VmSizerate){}
    Process(int pid,std::string name,ull VmSize,ull VmRSS):pid(pid),name(std::move(name)),VmSize(VmSize),VmRSS(VmRSS){}
    Process()= default;
    bool operator < (const Process &p)const{
        return VmSize > p.VmSize;
    }
};
ull getTotalMem();
void getMemInfo(std::mutex &mt,std::map<int,Process>&mySet,ull totalMem);
void getSuspiciousPid(std::vector<int> &list,std::map<int,Process>&mySet);
void traverseMemInfo(std::mutex &mt,std::map<int,Process>&mySet);
int detectCertainPid(std::map<int,Process>&mySet);

//Task2
struct compare_result{
    std::vector<std::string> before_result;
    std::vector<std::string> after_result;
};

std::vector<std::string>get_dir(const std::string& path);
std::vector<std::string> execute_cmd(const char *cmd);
std::vector<std::string> get_fd(int &pid);
std::vector<std::string> get_maps(int &pid);
compare_result compare_vector_string(std::vector<std::string> before, std::vector<std::string> after);
void print_compare_result(std::mutex &mt,int &pid,bool &flag);
void print_detect_static(int &pid);
#endif //OS_PROJ2_TASK_H
