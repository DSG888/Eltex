#ifndef PING_H
#define PING_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h> 
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>			//hostent, gethostbyname()
#include <errno.h>
#include <arpa/inet.h>		//inet_addr()
#include <signal.h>
#include <netinet/in.h>

#include <stdint.h>

#include "raw.h"

void alarm_handler(int);
void int_handler(int);
void set_sighandler();
void send_ping();
void recv_reply();
uint16_t checksum(uint8_t *buf, int len);
int handle_pkt();
void get_statistics(int, int);
void bail(const char *);

char *hostname;

char sendbuf[BUFSIZE];
char recvbuf[BUFSIZE];
int nsent;
int nrecv;
pid_t pid;
struct timeval recvtime;
int sockfd;
struct sockaddr_in dest;
struct sockaddr_in from;
struct sigaction act_alarm;
struct sigaction act_int;

#endif
