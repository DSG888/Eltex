#ifndef SHELL_H
#define SHELL_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#include "parser.h"

int fork1(void);
struct cmd* execcmd(void);
struct cmd* redircmd(struct cmd *subcmd, char *file, int type);
struct cmd* pipecmd(struct cmd *left, struct cmd *right);
void runcmd(struct cmd *cmd);
int getcmd(char *buf, int nbuf);

#endif
