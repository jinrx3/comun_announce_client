#ifndef __TCP_FILE_H__
#define __TCP_FILE_H__

#include "jpeg_show.h"
#include "ts.h"
#include "lcd.h"
#include "client_connect.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

//客户端发送图片文件
void tcp_send(char *pathname,int socket);

//客户端接收图片文件
void tcp_recv(char *pathname,int socket);


#endif
	