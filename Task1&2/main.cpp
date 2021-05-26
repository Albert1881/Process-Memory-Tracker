#include <iostream>
#include <unistd.h>
#include <thread>
#include "Task.h"
using namespace std;
void showMenu(){
    cout<<"Please enter the following number\n";
    cout<<"1: Show memory info of all process\n";
    cout<<"2: Show the current cpu usage\n";
    cout<<"3: Detect file handle and memory change of the program with certain pid\n";
    cout<<"4: Stop detecting file handle and memory change in 3\n";
    cout<<"5: Get the called stack info of the program with certain pid\n";
    cout<<"0: Exit!\n";
}
int main() {
    set<Process>mySet;
    set<int>pids;
    std::mutex mt;
    std::mutex mt_detect;
    ull totalMem = getTotalMem();
    thread tid_getInfo(getMemInfo,std::ref(mt),std::ref(mySet),std::ref(pids),totalMem);
    tid_getInfo.detach();
    string cmd;
    bool flag_detect = false;
    showMenu();
    while(cin>>cmd) {
        if(cmd == "-h"){
            showMenu();
        }
        else if(cmd == "1"){
            traverseMemInfo(mt,mySet);
        }
        else if(cmd == "2"){
            cout<<"cpu usage is: "<<get_cpu_usage()*100<<"%\n";
        }
        else if(cmd == "3"){
            cout<<"3: Detect file handle and memory change of the program with certain pid\n";
            mt_detect.lock();
            flag_detect = true;
            mt_detect.unlock();
            int pid = detectCertainPid(mySet,pids);
            if(pid != -1){
                thread tid_detect(print_compare_result,std::ref(mt_detect),std::ref(pid),std::ref(flag_detect));
                tid_detect.detach();
            }
        }
        else if(cmd == "4"){
            mt_detect.lock();
            cout<<"Close detect\n";
            flag_detect = false;
            mt_detect.unlock();
        }
        else if(cmd == "5"){
            cout<<"5: Get the called stack info of the program with certain pid\n";
            int pid = detectCertainPid(mySet,pids);
            if(pid != -1) {
                print_detect_static(pid);
            }
        }
        else if(cmd == "0"){
            exit(0);
        }
        else{
            printf("Wrong command\n");
            showMenu();
        }
    }
    return 0;
}
