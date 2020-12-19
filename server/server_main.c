#include "myhead.h"

int main(int argc,char *argv[])
{
	//初始化一条用户信息链表
	kl_pt head = kl_list_init();
	
	//获取返回的tcp_socket
	int tcp_socket = tcp_connect();
	
	//服务器管理函数
	server_manage(tcp_socket, head);
	
	return 0;
}