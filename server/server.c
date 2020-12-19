#include "myhead.h"

//服务器管理函数
int server_manage(int tcp_socket, kl_pt head)
{
	//启用多路复用
	int new_fd,max_fd=0;
	
	int dest_sockfd,size=0;
	
	//循环一次用的
	kl_pt get_node;
	struct list_head *pos,*n;
	//嵌套循环用的
	kl_pt get_node1;
	struct list_head *pos1,*n1;
	
	char recv[4096],broadcast[4096],snd[5120];
	system("clear");
	while(1)
	{
		printf("1.显示已连接用户\n2.发布公告\n");
		
		//清空文件描述符集合
		fd_set set;
		FD_ZERO(&set);
		//将需要监听的文件描述符添加到集合中
		FD_SET(0,&set);	//监听标准输入
		FD_SET(tcp_socket,&set);	//监听TCP服务器文件描述符
		//监听新链接进来的客户端描述符
		list_for_each_safe(pos,n,&head->list)
		{
			get_node = list_entry(pos, kl_st, list);
			FD_SET(get_node->sockfd,&set);
		}
		
		//处理最大文件描述符
		if(max_fd==0)
		{
			max_fd=tcp_socket;
		}
		else
		{
			//若不是0,说明有新用户链接进来，需要将它的new_fd赋给max_fd
			list_for_each_safe(pos,n,&head->list)
			{
				get_node = list_entry(pos, kl_st, list);
				if(get_node->sockfd > max_fd)
				{
					max_fd = get_node->sockfd;
				}
			}
		}
		
		//开始监听
		int ret_select = select(max_fd+1,&set,NULL,NULL,NULL);
		if(ret_select<0)
		{
			perror("select");
			return 0;
		}
		
		//判断是否为标准输入活跃
		if(FD_ISSET(0,&set))
		{
			char cmd[64]={0};
			scanf("%s",cmd);while(getchar()!='\n');
			if(!(strcmp(cmd,"1")))
			{
				kl_list_show(head);
			}
			else if(!(strcmp(cmd,"2")))
			{
				bzero(broadcast,4096);
				printf("请输入公告内容:\n");
				read(0, broadcast, 4096);
				list_for_each_safe(pos,n,&head->list)
				{
					get_node = list_entry(pos, kl_st, list);
					write(get_node->sockfd, broadcast, strlen(broadcast));
				}
				printf("公告已发送至各个客户端!\n");
			}
			else
			{
				printf("INVALID ARGUEMENT!\n");
			}
		}
		
		//判断是否为接受链接请求
		if(FD_ISSET(tcp_socket,&set))
		{
			new_fd=accept(tcp_socket,NULL,NULL);
			if(new_fd < 0)
			{
				perror("accept");
				return 0;
			}
			else
			{
				printf("new_fd=%d\n",new_fd);
				kl_list_add_tail(head, new_fd);
			}
		}
		
		//判断客户端是否有信息发送过来
		list_for_each_safe(pos,n,&head->list)
		{
			get_node = list_entry(pos, kl_st, list);
			if(FD_ISSET(get_node->sockfd,&set))
			{
				bzero(recv,4096);
				size = read(get_node->sockfd,recv,4096);
				if(size<=0)
				{
					printf("SocketFD为%d的客户端已掉线\n",get_node->sockfd);
					//删除已掉线的节点
					kl_list_del(head, get_node);
				}
				else
				{
					printf("Client(SOCKFD:%d):%s\n",get_node->sockfd,recv);
					//对收到的消息进行判断，若有对应信息则进行相应操作
					
					
					if(strstr(recv,"show"))	//发送当前好友列表给该用户
					{
						bzero(snd,4096);
						list_for_each_safe(pos1,n1,&head->list)
						{
							get_node1 = list_entry(pos1, kl_st, list);
							//通过 sockfd 获取网络描述符的信息  
							struct sockaddr_in  client_addr;
							uint32_t addr_len = sizeof(client_addr); 
							
							getpeername(get_node1->sockfd,(struct sockaddr *)&client_addr,&addr_len);
							char  *ip  = inet_ntoa(client_addr.sin_addr);
							int port = ntohs(client_addr.sin_port);
							
							//发送好友列表
							sprintf(snd,"UsrSockfd:%d	UsrIP:%s	UsrPORT:%d\r\n",get_node1->sockfd,ip,port);
							write(get_node->sockfd, snd, strlen(snd));
						}
					}
					else if(strstr(recv,"To"))	//接收到To,说明该用户要和别的用户聊天
					{
						sscanf(recv,"To %d",&dest_sockfd);
						//为了信息来源的可视性,加入了是哪个IP发送的操作
						struct sockaddr_in  client_addr;
						uint32_t addr_len = sizeof(client_addr);
						getpeername(get_node->sockfd,(struct sockaddr *)&client_addr,&addr_len);
						char *ip  = inet_ntoa(client_addr.sin_addr);
						
						//进行拼接
						sprintf(snd,"From %s :%s\r\n",ip,recv+5);
						write(dest_sockfd, snd, strlen(snd));
					}
					else if(strstr(recv,"send file"))	//接收到send file，说明该用户要传输文件
					{
						char accept_filepath[64]={0};
						sscanf(recv,"send file to %d %s",&dest_sockfd,accept_filepath);
						char *accept_file = strrchr(recv,'/')+1;
						printf("From Client[%d] send file:%s\n",get_node->sockfd,accept_file);
						char dest_file[64]={0};
						sprintf(dest_file,"./%s",accept_file);
						printf("Ready to send file:%s\n",accept_file);
						char header[64]={0};
						read(get_node->sockfd,header,64);
						int filesize=0;
						sscanf(header,"FileLength:%d",&filesize);
						printf("文件大小为%d字节\n",filesize);
						int down_size=0;
						char tmp[64]={0};
						sprintf(tmp,"send filesize %d file ./1.txt",filesize);
						write(dest_sockfd,tmp,strlen(tmp));
						while(1)
						{
							char buf[4096]={0};
							// 接受到文件内容的同时,就直接转发给目标用户
							size=read(get_node->sockfd,buf,4096);
							down_size+=size;
							write(dest_sockfd,buf,size); //转发给目标用户
							printf("已转发%d个字节\n",down_size);
							if(down_size==filesize)
							{
								printf("文件转发完毕\n");
								break;
							}
						}
					}
				}
			}
		}
	}
	return 0;
}

//新建一个返回tcp_socket的函数,以便操作返回的新用户new_fd
int tcp_connect()
{
	//创建TCP套接字
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	//解决端口复用
	int on=1;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	on=1;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEPORT,&on,sizeof(on));
	
	//地址结构体
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);

	bzero(&addr, len);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);  //自动获取当前ip地址
	addr.sin_port = htons(3017);

	//绑定
	bind(sockfd, (struct sockaddr *)&addr, len);

	//设置监听状态   fd 监听套接字
	listen(sockfd, 5);
	
	return sockfd;
}