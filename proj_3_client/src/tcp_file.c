#include "tcp_file.h"


//客户端发送图片文件
void tcp_send(char *pathname,int socket)
{
	//printf("---------5------------\n");
	//打开文件
	int file_fd = open(pathname,O_RDWR);
		if(file_fd < 0)
		{
			perror("文件打开失败！");
		}
		
	//获取文件大小
	int file_size = lseek(file_fd,0,SEEK_END);
	lseek(file_fd,0,SEEK_SET);
	
	char file_sz[20]={0};
	sprintf(file_sz,"%d",file_size);
	
	write(socket,file_sz,strlen(file_sz));
	
	sleep(1);
	
	//发送文件
	while(1)
	{
		char buf[4096]={0};
		
		int size = read(file_fd,buf,4096);
		
		if(size == 0)
		{
			
			break;
		}
		write(socket,buf,size);
	}

	printf("发送完毕！\n");
}


//客户端接收图片文件
void tcp_recv(char *pathname,int socket)
{
	//打开文件
	int file_fd = open(pathname,O_RDWR|O_CREAT|O_TRUNC,0777);
	if(file_fd < 0)
	{
		perror("");
		return ;
	}
	
	printf("等待发送方连接...\n");
	
	char file_sz[20]={0};
	read(socket,file_sz,20);
	
	int file_size = atoi(file_sz);	
	printf("file_size:%d\n",file_size);
	
	while(1)
	{
		char buf[4096]={0};
		int size = read(socket,buf,4096);
		write(file_fd,buf,size);
		
		//每次获取接收的文件大小
		lseek(file_fd,0,SEEK_SET);
		int file_size1 = lseek(file_fd,0,SEEK_END);
		if(file_size1 == file_size)
		{
			break;
		}
	}
	
	printf("接收成功！\n");
	close(file_fd);
}