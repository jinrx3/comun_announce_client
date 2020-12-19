#ifndef __CLIENT_CONNECT_H__
#define __CLIENT_CONNECT_H__

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

int tcp_connect(char *argv1, char *argv2);
int udp_connect();


#endif