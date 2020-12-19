#include "lcd.h"
// 全局变量
extern int lcd_fd;	//LCD设备文件描述符
extern int *lcd_p;
// 打开LCD设备
void open_lcd(void)
{
	lcd_fd = open("/dev/fb0", O_RDWR);
	if(lcd_fd == -1)
	{
		perror("open lcd failed");
		exit(0);
	}
	//2.对LCD 设备进行映射 
	lcd_p  = mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,lcd_fd,0); 
		 if(lcd_p == MAP_FAILED)
		 {
			 perror("");
			 return 0;
		 }
}

// 关闭LCD设备
void close_lcd(void)
{
	munmap(lcd_p,800*480*4);
	close(lcd_fd);
}