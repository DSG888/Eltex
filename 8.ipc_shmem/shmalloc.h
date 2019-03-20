#ifndef SHMALLOC_H
#define SHMALLOC_H

#include "main.h"
#include <pwd.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>

union semun 
{
	int val;
	struct semid_ds *buf;
	ushort *array;
};

#define MAXLINE 1024
#define MAXNAME 25
struct my_msgbufer
{
	long mtype;	
	char usrname[MAXNAME];
	char mtext[MAXLINE];
};

void main_handler(int flagmsq, int flagshm, int flagsem);
void cleanup();
void signalc(int s);

//global. fixme?
int qid, sid, mid;

#endif

