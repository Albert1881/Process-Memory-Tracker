//
// Created by gaosd on 2021/5/22.
//

#include <unistd.h>
#include <dirent.h>
#include <iostream>

using namespace std;
int main() {

    while (1) {
	sleep(3);
	int*a = new int[10000000];
	int*b = new int[10000000];
        cout << "process " << getpid() << endl;
        DIR *dir;
        dir = opendir("/proc");
    }
    return 0;
}
