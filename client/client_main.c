#include "myhead.h"

int main(int argc,char *argv[])
{
	//判断用户输入格式
	usage(argc,argv);
	
	//获取返回的tcp_socket
	int tcp_socket = tcp_connect(argv[1], argv[2]);
	
	//服务器管理函数
	client_manage(tcp_socket);
	
	return 0;
}