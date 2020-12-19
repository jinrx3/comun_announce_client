#include "main.h"

int lcd_fd;				//lcd设备文件描述符
int x, y, px, py;		//坐标和坐标偏移
int *lcd_p;
extern font *f;


//TCP线程
void *func_tcp(void *arg)
{
	int tcp_sock = *(int *)arg;
	//printf("---------4------------\n");
	while(1)
	{
		printf("输入show显示好友列表\n输入To <tcp_socket>:<消息内容>可与指定好友聊天\n输入send file to sockfd:<tcp_socket> 可传输文件给指定好友\n");
		char write_buf[1024]={0};
		bzero(write_buf,1024);
		fgets(write_buf,1024,stdin);
		write(tcp_sock,write_buf,sizeof(write_buf));
		if(strstr(write_buf,"send"))
		{
			tcp_send("./jpg/iu.jpg", tcp_sock);
		}	
		
	}
}

void *funcrv_tcp(void *arg)
{
	int tcp_sock = *(int *)arg;
	while(1)
	{
		char read_buf[1024]={0}; 
		bzero(read_buf,1024);
		int size1=read(tcp_sock,read_buf,1024);
		if(size1 <= 0)
		{
			printf("服务器已断开连接\n");
			break;
		}	
		printf("服务器发送的数据:read_buf=%s\n",read_buf);
		if(strstr(read_buf,"send"))
		{
			char file_path[50]={0};
			//printf("输入另存文件名：如：./jpg/xxx.jpg\n");
			//scanf("%s",file_path);
			tcp_recv("./jpg/xxx.jpg",tcp_sock);
			jpeg_show(file_path,0,0);
		}
		
		//字体大小的设置
		fontSetSize(f,32);
		
		//创建一个画板（点阵图）
		bitmap *bm = createBitmapWithInit(800,480,4,getColor(0,255,255,255));
		
		//将字体写到点阵图上
		fontPrint(f,bm,100,100,read_buf,getColor(0,255,0,0),630);
		
		//printf("buf=%s\n",read_buf);
		
		//把字体框输出到LCD屏幕上
		show_font_to_lcd(lcd_p,0,0,bm);
		
		//关闭画板
		destroyBitmap(bm);
	}
}

int main(int argc, char *argv[])
{	
	open_lcd();	//	打开lcd设备
	open_ts();	//	打开触摸屏
	f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");//  初始化字体库
	printf("--------1-------------\n");
	jpeg_show("./jpg/iu.jpg",0,0);
	
	//TCP连接
	int tcp_socket=tcp_connect(argv[1],argv[2]);
	pthread_t tid1,tid2;
	pthread_create(&tid2, NULL, funcrv_tcp, (void *)&tcp_socket);
	pthread_create(&tid1, NULL, func_tcp, (void *)&tcp_socket);
	
	int num=0;

	while(1)
	{
		get_xy(&x,&y,&px,&py);
		printf("(%d, %d)\n", px, py);
		
		if(px<400&&px>0 &&py<480&&px>0)
		{
			//广州天气
			http_rdata1("101280101",num);
			num=num+2;
			if(num == 14)
				num=0;
		}
		if(px<800&&px>400 &&py<480&&px>0)
		{
			//新闻
			http_rdata("toutiao");
		}
	}
	pthread_join(tid2,NULL);
	pthread_join(tid1,NULL);
	
	fontUnload(f);	//  关闭字体库
	close_ts();     //	关闭触摸屏
	close_lcd();	//	关闭lcd设备
	return 0;
}