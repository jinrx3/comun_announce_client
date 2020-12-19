#include "myhead.h"

//客户端管理函数
int client_manage(int tcp_socket)
{
	//启用多路复用
	int size=0;
	char recv[4096],snd[4096];
	system("clear");
	while(1)
	{
		printf("==================================================\n");
		printf("输入show显示好友链表\n输入To <socket_fd> <消息内容>可与指定好友聊天\n输入send file to <socket_fd>可传输文件给指定好友\n");
		printf("==================================================\n");
		//清空文件描述符集合
		fd_set set;
		FD_ZERO(&set);
		//将需要监听的文件描述符添加到集合中
		FD_SET(0,&set);	//监听标准输入
		FD_SET(tcp_socket,&set);	//监听TCP服务器文件描述符
		
		//开始监听
		int ret_select = select(tcp_socket+1,&set,NULL,NULL,NULL);
		if(ret_select<0)
		{
			perror("select");
			return 0;
		}
		
		//判断是否为标准输入活跃
		if(FD_ISSET(0,&set))
		{
			bzero(snd,4096);
			read(0,snd,4096);
			write(tcp_socket, snd, strlen(snd));
			
			//对发送给服务器的内容进行判断,如果有send file to <socket_fd>则发送文件给服务器转发给指定用户
			if(strstr(snd,"send file"))
			{
				//send file to 5 ./jpg/xxx.jpg	或者	send file to 5 ./xxx.txt
				char *src = strchr(snd,'.');
				char src_file[64]={0};
				strcpy(src_file,src);
				src_file[strlen(src_file)-1]='\0';
				printf("准备发送%s\n",src_file);
				int src_fd = open(src_file,O_RDWR,0777);
				if(src_fd<0)
				{
					perror("src_fd open failed");
					return 0;
				}
				int filelength=lseek(src_fd,0,SEEK_END);
				lseek(src_fd,0,SEEK_SET);
				bzero(snd,4096);
				sprintf(snd,"FileLength:%d",filelength);
				printf("%s\n",snd);
				write(tcp_socket,snd,strlen(snd));
				while(1)
				{
					bzero(snd,4096);
					size=read(src_fd,snd,4096);
					if(size<=0)
					{
						printf("文件发送完毕!\n");
						break;
					}
					write(tcp_socket,snd,strlen(snd));
				}
			}
			
		}
		
		//判断服务器是否有信息发送过来
		if(FD_ISSET(tcp_socket,&set))
		{
			bzero(recv,4096);
			size = read(tcp_socket,recv,4096);
			if(size<=0)
			{
				printf("服务器已停止通信,请稍后重新尝试连接!\n");
				return 0;
			}
			else if(strstr(recv,"send file"))
			{
				//send file to 5 ./jpg/xxx.jpg	或者	send file to 5 ./xxx.txt
				//分割获取到的字符，直接将发送过来的文件名作为存放的新文件名，省去判断是什么类型的文件
				int filesize=0;
				sscanf(recv,"send filesize %d file",&filesize);
				printf("接收到的文件大小:%d\n",filesize);
				char *dest_file = strrchr(recv,'/')+1;
				printf("接收到的文件:%s\n",dest_file);
				int dest_fd = open(dest_file,O_RDWR|O_CREAT|O_TRUNC,0777);
				if(dest_fd<0)
				{
					perror("dest_fd");
					return 0;
				}
				while(1)
				{
					char buf[4096]={0};
					size=read(tcp_socket,buf,4096);
					write(dest_fd,buf,size);
					lseek(dest_fd,0,SEEK_SET);
					int fsize=lseek(dest_fd,0,SEEK_END);
					printf("fsize=%d\n",fsize);
					if(fsize == filesize)
					{
						printf("文件接收完毕\n");
						break;
					}
				}
			}
			else
			{
				printf("[Server]:%s\n",recv);
			}
		}
	}
	return 0;
}

//整型转化成字符串
char* itoa(int num,char* str,int radix)
{
    char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";//索引表
    unsigned unum;//存放要转换的整数的绝对值,转换的整数可能是负数
    int i=0,j,k;//i用来指示设置字符串相应位，转换之后i其实就是字符串的长度；转换后顺序是逆序的，有正负的情况，k用来指示调整顺序的开始位置;j用来指示调整顺序时的交换。
 
    //获取要转换的整数的绝对值
    if(radix==10&&num<0)//要转换成十进制数并且是负数
    {
        unum=(unsigned)-num;//将num的绝对值赋给unum
        str[i++]='-';//在字符串最前面设置为'-'号，并且索引加1
    }
    else unum=(unsigned)num;//若是num为正，直接赋值给unum
 
    //转换部分，注意转换后是逆序的
    do
    {
        str[i++]=index[unum%(unsigned)radix];//取unum的最后一位，并设置为str对应位，指示索引加1
        unum/=radix;//unum去掉最后一位
 
    }while(unum);//直至unum为0退出循环
 
    str[i]='\0';//在字符串最后添加'\0'字符，c语言字符串以'\0'结束。
 
    //将顺序调整过来
    if(str[0]=='-') k=1;//如果是负数，符号不用调整，从符号后面开始调整
    else k=0;//不是负数，全部都要调整
 
    char temp;//临时变量，交换两个值时用到
    for(j=k;j<=(i-1)/2;j++)//头尾一一对称交换，i其实就是字符串的长度，索引最大值比长度少1
    {
        temp=str[j];//头部赋值给临时变量
        str[j]=str[i-1+k-j];//尾部赋值给头部
        str[i-1+k-j]=temp;//将临时变量的值(其实就是之前的头部值)赋给尾部
    }
 
    return str;//返回转换后的字符串
 
}

//TCP连接
int tcp_connect(char *argv1, char *argv2)
{
	//创建TCP套接字
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	//地址结构体
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(argv1);
	addr.sin_port = htons(atoi(argv2));

	//向服务器发起连接请求
	connect(sockfd, (struct sockaddr *)&addr, len);
	return sockfd;
}

//判断用户输入格式
void usage(int argc, char **argv)
{
	if(argc != 3)
	{
		printf("程序用法：%s <IP> <PORT> \n", argv[0]);
		exit(0);
	}
}