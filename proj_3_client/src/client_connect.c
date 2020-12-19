#include "client_connect.h"


int tcp_connect(char *argv1, char *argv2)
{
	//1.创建 TCP  通信协议
	int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(tcp_socket < 0)
	{
		perror("");
		return 0; 
	}
	else
	{
		printf("创建成功\n");
	}
	//解决端口复用
	int on=1;
	setsockopt(tcp_socket,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	//on=1;
	//setsockopt(tcp_socket,SOL_SOCKET,SO_REUSEPORT,&on,sizeof(on));
	
	//设置链接的服务器地址信息 
	struct sockaddr_in  addr;  
	addr.sin_family   = AF_INET; //IPV4 协议  
	addr.sin_port     = htons(atoi(argv2)); //端口
	addr.sin_addr.s_addr = inet_addr(argv1); //服务器的IP 地址
	//2.链接服务器 
	int ret=connect(tcp_socket,(struct sockaddr *)&addr,sizeof(addr));
	if(ret < 0)
	{
		perror("");
		return 0;
	}
	else
	{
		printf("链接服务器成功\n");
	}
	
	return tcp_socket;
	
}

//UDP通信
int udp_connect()
{
	//1.创建UDP  通信socket  
	int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(udp_socket < 0)
	{
	  perror("");
	  return 0;
	}
	
	//解决端口复用
	int on=1;
	setsockopt(udp_socket,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	//on=1;
	//setsockopt(udp_socket,SOL_SOCKET,SO_REUSEPORT,&on,sizeof(on));	
	struct sockaddr_in  dest_addr;  
	dest_addr.sin_family   = AF_INET; //IPV4 协议  
	dest_addr.sin_port     = htons(7777); //端口
	dest_addr.sin_addr.s_addr = INADDR_ANY; //绑定本地网卡
	//2.绑定服务器地址信息
	int ret=bind(udp_socket,(struct sockaddr *)&dest_addr,sizeof(dest_addr));
	if(ret < 0)
	{
		perror("");
		return 0;
	}
	else
	{
		printf("绑定成功\n");
	}
	
	return udp_socket;
}

