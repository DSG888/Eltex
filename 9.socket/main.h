#ifndef MAIN_H
#define MAIN_H


#include <stdio.h>		//fprintf
#include <stdlib.h>		//exit srand EXIT_SUCCESS EXIT_FAILURE
#include <string.h>		//strlen strchr
#include <pthread.h>	//pthread_create
#include <netdb.h>		//gethostbyname hostent
#include <arpa/inet.h>	//inet_ntoa
#include <signal.h>		//signal
#include <unistd.h>		//sleep

#include "other.h"
#include "tcp.h"
#include "udp.h"

#define server_key	"Ss"
#define client_key	"Cc"
#define tcp_key		"Tt"
#define udp_key		"Uu"
#define TCP_PROTO	1
#define UDP_PROTO	2
#define SERVER_MODE	3
#define CLIET_MODE	4
#define DEFAULT_PORT 62000
#define DEFAULT_IP	"127.0.0.1"
#define MAXPENDING	10

struct main_handler_args_s {	// Это можно было бы заменить более развесистой структурой struct sockaddr_in
	unsigned int ip;
	int port;
};

struct connect_node_s {
	unsigned int num;
	int sock;
	pthread_t client_threadID;
} typedef connect_node_t;

#endif

