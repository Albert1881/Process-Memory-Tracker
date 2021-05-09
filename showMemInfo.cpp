#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#define MAX 1024
#define PATH_SIZE 128
using namespace std;
int main(void)
{
	DIR *dir;
	struct dirent *entry;
	FILE *fp;
	char path[PATH_SIZE];
	char buf[MAX];
	printf("NAME\t\t\tPID\t\tMem\n"); /* 输出表头 */
	if((dir = opendir( "/proc" )) == NULL ) { /* 打开/proc目录 */
		perror("fail to open dir");
			return -1; 
	}
	while((entry = readdir( dir ) ) != NULL){
		if(entry->d_name[0] == '.') /* 跳过当前目录，proc目录没有父目录 */ 
			continue;
		/* 跳过系统信息目录，所有进程的目录全都是数字，而系统信息目录全都不是数字 */
		if( (entry->d_name[0] <='0' ) || (entry->d_name[0] >= '9'))
			continue;
		/* 使用sprintf完成拼接路径，其中两个%s会由entry->d_name表示的进程ID替
		代 */
		sprintf(path, "/proc/%s/task/%s/status", entry->d_name,entry->d_name);
		fp = fopen(path, "r"); /* 打开文件 */
		if(fp == NULL){
			perror("fail to open");
			exit(1);
		}
        bool flag = false;
		while(fgets(buf, MAX, fp) != NULL){ /* 读取每一行 */
			if(buf[0] == 'N'
 			&& buf[1] == 'a'
			&& buf[2] == 'm'
			&& buf[3] == 'e')
			{
				int i=6;
				while(buf[i]!='\n')
				{
					printf("%c", buf[i]); /* 跳过‘\n’，输出状态信息 */
					i++;
				}
                printf("\t\t");
			}
			if(buf[0] == 'P'
			&& buf[1] == 'i'
			&& buf[2] == 'd'){
                int i = 5;
				while(buf[i]!='\n')
				{
					printf("%c", buf[i]); /* 跳过‘\n’，输出状态信息 */
					i++;
				}
			}
            if(buf[0] == 'V'
			&& buf[1] == 'm'
			&& buf[2] == 'R'
            && buf[3] == 'S'
            && buf[4] == 'S'){
                int i = 7;
				while(buf[i]!='\n')
				{
					printf("%c", buf[i]); /* 跳过‘\n’，输出状态信息 */
					i++;
				}
                printf("\n");
                flag = true;
                break;
			}
		}
        if(!flag)
            printf("\n");
		fclose(fp); /* 关闭stattus文件 */
	}
	closedir( dir ); /* 关闭目录 */
	return 0;
}