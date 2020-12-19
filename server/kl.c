#include "myhead.h"		//内核链表头文件

// 初始化内核链表头节点
kl_pt kl_list_init(void)
{
	// 1.申请堆空间给头节点
	kl_pt h = malloc(sizeof(kl_st));
	if(h == NULL)
	{
		perror("head malloc failed");
		return NULL;
	}
	
	// 2.使其指针域，都指向自身
	// h->list.next = &(h->list);
	// h->list.prev = &(h->list);
	INIT_LIST_HEAD(&h->list);
	
	// 3.将头节点地址返回
	return h;
}

//释放所有节点堆空间
void kernel_list_exit(kl_pt head)
{
	kl_pt get_node;
	int count=1;
	struct list_head *pos,*n;
	list_for_each_safe(pos,n,&head->list)
	{
		get_node = list_entry(pos, kl_st, list);
		free(get_node);
		count++;
	}
	free(head);
	printf("一共释放了%d个节点堆空间！\n",count);
}

// 添加数据到内核链表尾
void kl_list_add_tail(kl_pt head, int new_data)
{
	// 1.申请堆空间给新节点，清空并把数据给入
	kl_pt new_node = malloc(sizeof(kl_st));
	if(new_node == NULL)
	{
		perror("new_node malloc failed");
		return;
	}
	memset(new_node, 0, sizeof(kl_st));
	new_node->sockfd = new_data;
	// 2.调用内核链表函数添加函数
	list_add_tail(&new_node->list, &head->list);
}

// 删除指定数据
void kl_list_del(kl_pt head, kl_pt del_data)
{
	// 0.判断是否为空表
	if(list_empty(&head->list))
	{
		printf("Empty!!!\n");
		return;
	}
	
	// 1.遍历，对比查找指定数据
	struct list_head *pos, *n;
	kl_pt get_node;
	list_for_each_safe(pos, n, &head->list)
	{
		// 找到了，记录下标，跳出循环
		get_node = list_entry(pos, kl_st, list);
		if(get_node->sockfd==del_data->sockfd)
			break;
	}
	
	// 没找到，提示并退出
	if(pos == &head->list)
	{
		printf("Not Found!!\n");
		return;
	}
	
	//将节点删除并释放
	list_del(pos);
	free(get_node);
	
	return;
}

// 显示链表数据
void kl_list_show(kl_pt head)
{
	//遍历链表
	kl_pt get_node;
	
	struct list_head *pos;	//遍历小结构体
	printf("\n===============SHOW BEGIN===============\n");
	list_for_each(pos, &head->list)
	{
		// 通过小结构体地址，获得大结构体地址
		get_node = list_entry(pos, kl_st, list);
		
		//通过 sockfd 获取网络描述符的信息  
		struct sockaddr_in  client_addr;
		uint32_t addr_len = sizeof(client_addr); 
		
		getpeername(get_node->sockfd,(struct sockaddr *)&client_addr,&addr_len);
		char  *ip  = inet_ntoa(client_addr.sin_addr);
		int port = ntohs(client_addr.sin_port);
		
		
		printf("sockfd=%d	ip:%s	port:%d\n",get_node->sockfd,ip,port);
		
		printf("==============================\n");
	}
	printf("===============SHOW OVER===============\n\n");
}