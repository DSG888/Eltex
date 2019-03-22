#include "shell.h"


int fork1(void) {
	int pid;
	pid = fork();
	if(pid == -1)
		perror("fork");
	return pid;
}

struct cmd* execcmd(void) {
	struct execcmd *cmd;
	cmd = malloc(sizeof(*cmd));
	memset(cmd, 0, sizeof(*cmd));
	cmd->type = ' ';
	return (struct cmd*)cmd;
}

struct cmd* redircmd(struct cmd *subcmd, char *file, int type) {
	struct redircmd *cmd;
	cmd = malloc(sizeof(*cmd));
	memset(cmd, 0, sizeof(*cmd));
	cmd->type = type;
	cmd->cmd = subcmd;
	cmd->file = file;
	cmd->mode = (type == '<') ?  O_RDONLY : O_WRONLY|O_CREAT|O_TRUNC;
	cmd->fd = (type == '<') ? 0 : 1;
	return (struct cmd*)cmd;
}

struct cmd* pipecmd(struct cmd *left, struct cmd *right) {
	struct pipecmd *cmd;
	cmd = malloc(sizeof(*cmd));
	memset(cmd, 0, sizeof(*cmd));
	cmd->type = '|';
	cmd->left = left;
	cmd->right = right;
	return (struct cmd*)cmd;
}

// Выполняет команду
void runcmd(struct cmd *cmd) {
	int p[2];
	struct execcmd *ecmd;
	struct pipecmd *pcmd;
	struct redircmd *rcmd;
	if(cmd == 0)
		exit(0);
	switch(cmd->type) {
		default:
			fprintf(stderr, "unknown runcmd\n");
			exit(-1);
		case ' ':
			ecmd = (struct execcmd*)cmd;
			if(ecmd->argv[0] == 0)
				exit(0);
			execv(ecmd->argv[0], ecmd->argv);
			break;
		case '>':
		case '<':
			rcmd = (struct redircmd*)cmd;
			close(rcmd->fd);		//закр дискр
			open(rcmd->file, rcmd->mode, 0666);		//открытие нового
			runcmd(rcmd->cmd);
			break;
		case '|':
			pcmd = (struct pipecmd*)cmd;
			if (pipe(p) < 0) {
			//	fpintf("error\n");
				break;
			}
			if (fork() == 0) {
				close(1);
				dup(p[1]);
				close(p[0]);
				close(p[1]);
				runcmd(pcmd->left);
			}
			if (fork() == 0) {
				close(0);
				dup(p[0]);
				close(p[0]);
				close(p[1]);
				runcmd(pcmd->right);
			}
			close(p[0]);
			close(p[1]);
			wait(NULL);
			wait(NULL);
			break;
	}
	exit(0);
}

int getcmd(char *buf, int nbuf) {
	if (isatty(fileno(stdin)))
		fprintf(stdout, "$ ");
	memset(buf, 0, nbuf);
	fgets(buf, nbuf, stdin);
	if(buf[0] == 0) // EOF
		return -1;
	return 0;
}


