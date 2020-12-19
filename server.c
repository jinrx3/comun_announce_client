#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


struct  node 
{
    int socket;  	
	struct node*  prev;
	struct node*  next;
};  

//初始化双向链表
struct  node * link_list_init(void)
{
	// a.给头节点分配堆空间
	struct  node * mylist = malloc(sizeof(struct  node));
	if(mylist == NULL)
	{
		perror("malloc error!");
		return NULL;
	}
	// b.使next指向头节点本身，prev指向本身
	mylist->next = mylist;
	mylist->prev = mylist;
	
	// c.返回头节点地址
	return mylist;
}

void  inser_node(struct node  *head,int socket)
{
	//1.新建结点  
	struct node  *new =  malloc(sizeof(struct node));
				  new->socket = socket;  
				  
	//2.插入  
	new->prev = head->prev; 
	new->next = head;  
	head->prev->next = new;  
	head->prev = new; 
}

// 判断是否为空链表(只有头节点，没有数据节点)
bool is_empty(struct  node * head)
{
	return head->next==head || head->prev==head;
}


// 删除链表中的数据
bool link_list_del(struct  node * head, struct  node * del)
{
	// 0.判断是否为空链表
	if(is_empty(head))
	{
		printf("Empty!!\n");
		return false;
	}
	// 1.遍历链表
		// 找到了，记录下标，跳出循环
		// 没找到，提示用户，并返回
	struct  node * pos;
	for(pos=head->next; pos!=head; pos=pos->next)
	{
		if(pos == del)
			break;
	}
	if(pos == head)
	{
		printf("not found!\n");
		return false;
	}
	// 2.修改前后节点指向（顺序可调换）
		// 2.1 操作前节点next，使其指向后节点
	//前节点->next = 后节点;
	pos->prev->next = pos->next;
		// 2.2 操作后节点prev，使其指向前节点
	//后节点->prev = 前节点;
	pos->next->prev = pos->prev;
	pos->next = NULL; 
	pos->prev = NULL;  
	
	return true;
}


int main()
{
	//创建tcp套接字
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd==0)
	{
		perror("");
		return -1;
	}
	//SO_REUSEADDR
	//解决端口复用
	int on=1;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	on=1;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEPORT,&on,sizeof(on));
	
	//绑定自己的IP信息
	struct sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(10086);//端口号
	addr.sin_addr.s_addr=INADDR_ANY;
    int ret=bind(sockfd,(struct sockaddr *)&addr,sizeof(addr));	
	if(ret<0)
	{
		perror("bind");
		return -1;
		
	}
	
	//监听
	listen(sockfd,6);
	
	//创建链表头结点
	struct node *head =link_list_init();
	head->socket=0;
	
	int newfd =0;
	
	int max = sockfd; //最大的描述符
	struct node* pos=NULL;
	struct node* tmp=NULL;
	
	char announcement[100]={"hello"};
	while(1)
	{
	//1.把文件描述符集合清空   （先把监狱处理干净）
		fd_set set;
		FD_ZERO(&set);//清空文件描述集合 
	
	//2.把阻塞的文件描述符添加到集合中  
		FD_SET(sockfd,&set);
		FD_SET(0,&set);
		for(pos=head->next; pos!=head; pos=pos->next)
		{
			FD_SET(pos->socket,&set);
		}
		
	//3.监听集合        （看看谁不听话）
	   int ret = select(max+1,&set,NULL,NULL,NULL);//设置监听的时长 
		if(ret < 0)
		{
			perror("");
			return 0;
		}	
		
		if(FD_ISSET(0,&set))	
		{
			bzero(announcement,sizeof(announcement));
			read(0,announcement,sizeof(announcement));
			
			for(pos=head->next; pos!=head; pos=pos->next)
			{
				write(pos->socket,announcement,strlen(announcement));
			}
			printf("新公告发布成功\n");
		}
		
		if(FD_ISSET(sockfd,&set))  //判断是否  服务器socket  活跃 
		{
			newfd=accept(sockfd,NULL,NULL);
			if(newfd>0)
			{
				printf("链接成功 newfd=%d\n",newfd);
				max = newfd;
				inser_node(head,newfd);
				write(newfd,announcement,strlen(announcement));
			}
		}
		for(pos=(head)->next,tmp=pos->next;pos!=(head);pos=tmp,tmp=pos->next)
		{
			if(FD_ISSET(pos->socket,&set))   //判断是否已连接用户的描述符活跃 
			{
				char  buf[1024]={0};
				int ret = read(pos->socket,buf,1024);
				if(ret <= 0)
				{
					printf("客户端已经掉线了\n");
					link_list_del(head,pos);//用户断开连接，删除用户数据
					close(pos->socket);
					free(pos);
				}
				else
				{
					printf("Client(SOCKFD:%d):%s\n",pos->socket,buf);
					//对收到的消息进行判断，若有对应信息则进行相应操作
					
					
					if(strstr(buf,"show"))	//发送当前好友列表给该用户
					{
						char snd[4096]={0};
						struct node* pos1=NULL;
						for(pos1=head->next; pos1!=head; pos1=pos1->next)
						{
							bzero(snd,4096);
							//通过 sockfd 获取网络描述符的信息  
							struct sockaddr_in  client_addr;
							int addr_len = sizeof(client_addr); 
							
							getpeername(pos1->socket,(struct sockaddr *)&client_addr,&addr_len);
							char  *ip  = inet_ntoa(client_addr.sin_addr);
							int port = ntohs(client_addr.sin_port);
							
							//发送好友列表
							sprintf(snd,"UsrSockfd:%d	UsrIP:%s	UsrPORT:%d\n",pos1->socket,ip,port);
							write(pos->socket, snd, strlen(snd));
						}
					}
					else if(strstr(buf,"To"))	//接收到To,说明该用户要和别的用户聊天
					{
						char snd[4096]={0};
						int dest_sockfd;
						sscanf(buf,"To %d",&dest_sockfd);
						//为了信息来源的可视性,加入了是哪个IP发送的操作
						struct sockaddr_in  client_addr;
						int addr_len = sizeof(client_addr);
						getpeername(pos->socket,(struct sockaddr *)&client_addr,&addr_len);
						char *ip  = inet_ntoa(client_addr.sin_addr);
						
						//进行拼接
						sprintf(snd,"From %s :%s\n",ip,buf+5);
						write(dest_sockfd, snd, strlen(snd));
					}
					else if(strstr(buf,"send file"))	//接收到send file，说明该用户要传输文件
					{
						int dest_sockfd;
						sscanf(buf,"send file to sockfd:%d",&dest_sockfd);
						write(dest_sockfd,"send",strlen("send"));

						/* int file_fd = open("./1.jpg",O_RDWR|O_CREAT|O_TRUNC,0777);
							if(file_fd < 0)
							{
								perror("");
								return 0;
							} */
						while(1)
						{
							// 接受到文件内容的同时,就直接转发给目标用户
							char file_buf[4096]={0};
							int size=read(pos->socket,file_buf,4096);
							
							
							if(strstr(file_buf,"over"))
							{
								printf("文件转发完毕\n");
								break;
							}
							
							write(dest_sockfd,file_buf,size); //转发给目标用户
							//printf("%s\n",file_buf);
							//write(file_fd,file_buf,size);	
							
						}
						//close(file_fd);
					}
				}
			}
		}
	}
}
