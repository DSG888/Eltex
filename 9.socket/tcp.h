#ifndef TCP_H
#define TCP_H

#include <stdio.h>		//fprintf
#include <pthread.h>	//pthread_detach pthread_self
#include <sys/socket.h>	//recv send
#include <unistd.h>		//sleep
#include <string.h>		//memset

#include "other.h"
#include "main.h"

int create_server_tcp_socket(unsigned int ip, int port);
int accept_tcp_connection(int servSock);
int client_tcp_connect(unsigned int ip, int port);
void* server_worker_tcp(void *threadArgs);
void* server_handler_tcp(void *threadArgs);
void* client_handler_tcp(void *threadArgs);

#endif
