#include "parser.h"
#include "shell.h"

/*
	Напиши теперь комадный интерпритатор типа bash, пользователь вводит 
команду, интерпритатор ее запускает на выполнение (fork/exec) и ожидает 
ее завершения (wait), выводит статус завершения.
*/
// Основано на xv6 shell.


int main(void) {
	static char buf[100];
	int r;
	// Чтение команд
	while(getcmd(buf, sizeof(buf)) >= 0) {
		if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' ') {
			buf[strlen(buf) - 1] = 0;  // \n
			if(chdir(buf+3) < 0)
				fprintf(stderr, "cannot cd %s\n", buf + 3);
			continue;
		}
		if(fork1() == 0)
			runcmd(parsecmd(buf));
		wait(&r);
		printf("Код завершения: %d\n", r>>8);
	}
	exit(0);
}

