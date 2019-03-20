#ifndef UPD_H
#define UPD_H

#include <stdio.h>		//fprintf
#include <pthread.h>	//pthread_detach pthread_self
#include <sys/socket.h>	//sendto recvfrom
#include <unistd.h>		//sleep
#include <string.h>		//memset

#include "other.h"
#include "main.h"

#define MAGIC_NUM 0xAAAA	// Магический запрос или 0b1010101010101010

int create_server_udp_socket(unsigned int ip, int port, int binding);
void* client_handler_udp(void *threadArgs);
void* server_handler_udp(void *threadArgs);

#endif







