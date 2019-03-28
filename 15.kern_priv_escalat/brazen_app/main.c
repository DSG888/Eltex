#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void sighandler(int sig) {
	if (sig == SIGINT) {
		fprintf(stdout, "Пробуем открыть /etc/shadow: ");
		FILE* f = NULL;
		//int buf = '\0';
		if (!(f = fopen("/etc/shadow", "rt")))
			fprintf(stdout, "Не открывается\n");
		else
			fprintf(stdout, "Открывается\n");
	}
}

int main(int argc, char **argv) {
	if (signal(SIGINT, sighandler) == SIG_ERR) {
		fprintf(stderr, "signal\n");
		exit(1);
	}
	while (1)
		sleep(1);
}
