#ifndef __HTTP_RDATA_H
#define __HTTP_RDATA_H

#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <strings.h>
#include "client_connect.h"
#include "font.h"
#include "lcd.h"
#include "cJSON.h"

void http_rdata(char *data);
void http_rdata1(char *data,int num);



#endif