#ifndef _MYHEAD_H
#define _MYHEAD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <dirent.h>
#include <linux/input.h>	//输入子系统头文件
#include <dirent.h>
#include <pthread.h>
#include <sys/wait.h>
#include <netinet/ip.h> /* superset of previous */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 

// 函数声明

//TCP连接
int tcp_connect(char *argv1, char *argv2);

//客户端管理函数
int client_manage(int tcp_socket);

//判断用户输入格式
void usage(int argc, char **argv);

//整型转化成字符串
char* itoa(int num,char* str,int radix);

#endif