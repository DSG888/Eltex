#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "shell.h"

#define MAXARGS 10
#define whitespace " \t\r\n\v"
#define symbols "<|>"

struct cmd {
	int type;				//  ' ' (exec), | (pipe), '<' or '>' для перенаправления
};

struct execcmd {
	int type;				// ' '
	char *argv[MAXARGS];	// аргументы для команды, которая будет выполнена
};

struct redircmd {
	int type; 
	struct cmd *cmd;
	char *file;
	int mode;
	int fd;
};

struct pipecmd {
	int type;
	struct cmd *left;
	struct cmd *right;
};

struct cmd* parsecmd(char *s);
struct cmd* parseline(char **ps, char *es);
struct cmd* parsepipe(char **ps, char *es);
struct cmd* parseredirs(struct cmd *cmd, char **ps, char *es);
struct cmd* parseexec(char **ps, char *es);
char *mkcopy(char *s, char *es);
int peek(char **ps, char *es, char *toks);
int gettoken(char **ps, char *es, char **q, char **eq);

#endif
