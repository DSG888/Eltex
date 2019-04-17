#ifndef NET_H
#define NET_H

#define MAXSERVERS 10	// Максимум серверов

#include <stdint.h>

#include <sys/socket.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
//#include <alloca.h>


#include "main.h"

struct srv_handler_s {
	uint32_t ip;
	uint16_t port;

};

struct endpoint_s {
	uint32_t ip;
	uint16_t port;
} typedef endpoint_t;

#define MAGICNUM 0xAAAAAAAA
struct broadcastanonce_s {
	uint32_t magic1;	//AAAAAAAA
	endpoint_t masterknot;
	endpoint_t myep;
	volatile endpoint_t servertable[MAXSERVERS - 1];
	uint32_t magic2;	//AAAAAAAA
} typedef broadcastanonce_t;

struct server_s {
	broadcastanonce_t anonce;
	endpoint_t* servertable;
	int server_socks[MAXSERVERS - 1];
} typedef server_t;

//#include ""

int getipaddr(char* ifname, struct in_addr* ip);
int getbroadcastaddr(char* ifname, struct in_addr* bc);
void* broadcast_handler_tr(void *args);
void* broadcast_handler_srv(void *args);
int create_server_socket(uint32_t ip);
int client_connect(endpoint_t ep);

#endif
