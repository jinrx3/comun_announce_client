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
#include "list.h"
#include <pthread.h>
#include <sys/wait.h>
#include <netinet/ip.h> /* superset of previous */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 

// 数据节点结构体声明
typedef struct kernel_list{
	// 1.数据域
	int sockfd;
	// 2.指针域（小结构体）
	struct list_head list;
}kl_st, *kl_pt;

// 函数声明
// 初始化内核链表头节点
kl_pt kl_list_init(void);

// 添加数据到内核链表尾
void kl_list_add_tail(kl_pt head, int new_data);

// 删除指定数据
void kl_list_del(kl_pt head, kl_pt del_data);

// 显示链表数据
void kl_list_show(kl_pt head);

//释放所有节点堆空间
void kernel_list_exit(kl_pt head);

//新建一个返回tcp_socket的函数,以便操作返回的新用户new_fd
int tcp_connect();

//服务器管理函数
int server_manage(int tcp_socket, kl_pt head);

#endif