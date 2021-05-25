#include <iostream>
#include <thread>
#include "Task.h"
using namespace std;
std::mutex mt;
std::set<Process>mySet;
std::set<int>pids;
int main() {
    set<Process>mySet;
    set<int>pids;
    std::mutex mt;
    ull totalMem = getTotalMem();
    thread tid(getMemInfo,std::ref(mt),std::ref(mySet),std::ref(pids),totalMem);
    string cmd;
    while(cin>>cmd) {
        if(cmd == "show"){
            traverseMemInfo(mt,mySet);
        }
        else if(cmd == "detect"){
            detectCertainPid(mySet,pids);
        }
        else if(cmd == "quit"){
            exit(0);
        }
        else{
            printf("Wrong command\n");
        }
    }
    return 0;
}
