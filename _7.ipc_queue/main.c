// Разработать межпроцессный чат с использованием IPC

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

#define MAXLEN 256
typedef struct {
	uint8_t mtype;
	char mtext[MAXLEN];
} msg_t;

void* recivemsg_handler() {
	while(1) {
		msg_t recmsg;
		int msgrec;
		key_t key = (int)getpid();
		int msqid;
		msqid = msgget(key,IPC_CREAT | 0666);
		if(msqid == -1) {
			perror("msgget");
			exit(EXIT_FAILURE);
		}
		msgrec = msgrcv(msqid, &recmsg, sizeof recmsg.mtext, 0, 0);
		if (msgrec == -1) {
			if (errno == EIDRM)
				return EXIT_SUCCESS;
			perror("msgrec");
			exit(EXIT_FAILURE);
		}
		printf("%s\n",recmsg.mtext );
	}
}

int main(int argc, char * argv[]) {
	pthread_t receivethread;
	if((pthread_create(&receivethread, NULL, recivemsg_handler, (void *)NULL)) != 0) {
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}
	key_t key = (int)getpid();
	int msqid;
	msqid = msgget(key, IPC_CREAT | 0666);
	if (msqid == -1) {
		perror("msgget");
		exit(EXIT_FAILURE);
	}
	char * pid = malloc(8);
	snprintf(pid, 16, "%i", getpid());
	char * sentstring = malloc(32);
	sentstring[0] = '\0';
	sentstring = strcat(sentstring, ">");
	sentstring = strcat(sentstring, pid);
	sentstring = strcat(sentstring, ": ");	//FIXME?
	while(1) {
		char *line = NULL;
		char *str = NULL;
		int length, maxind, ind, msqidint;
		struct msqid_ds ds;
		struct msginfo info;
		msg_t msg;
		size_t len = 0;
		getline(&line, &len, stdin);
		line[strlen(line) -1] =0;
		line[len] = '\0';
		if ((str = malloc(strlen(sentstring) + strlen(line) + 1)) != NULL) {
			str[0] = '\0';
			str = strcat(str, sentstring);
			str = strcat(str, line);
		}
		//msg.mtype = 100;
		strcpy(msg.mtext, str);
		free(str);
		length = strlen(msg.mtext);
		msg.mtext[length] = '\0';
		maxind = msgctl(0, MSG_INFO, (struct msqid_ds*)&info);
		if (maxind == -1) {
			perror("msgctl");
			exit(EXIT_FAILURE);
		}
		for(ind = 0; ind <= maxind; ind++) {
			msqidint = msgctl(ind, MSG_STAT, &ds);
			if (msqidint == -1)
				if (errno != EINVAL && errno != EACCES)
					continue;
			if (!((int)ds.msg_perm.__key == (int)getpid())) {
				int tempmsqid = msgget(ds.msg_perm.__key,IPC_CREAT | 0666);
				if (msgsnd(tempmsqid, &msg, 1 + length, 0) == -1) {
					perror("msgsnd");
					exit(EXIT_FAILURE);
				}
			}
		}
	}
	if (msgctl(msqid, IPC_RMID, NULL) == -1) {
		perror("msgctl");
		exit(EXIT_FAILURE);
	}
	if (pthread_join(receivethread, NULL) != 0) {
		perror("pthread_join");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}

