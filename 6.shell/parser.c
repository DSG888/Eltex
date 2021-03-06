#include "parser.h"

struct cmd* parsecmd(char *s) {
	char *es;
	struct cmd *cmd;
	es = s + strlen(s);
	cmd = parseline(&s, es);
	peek(&s, es, "");
	if(s != es) {
		fprintf(stderr, "leftovers: %s\n", s);
		exit(-1);
	}
	return cmd;
}

struct cmd* parseline(char **ps, char *es) {
	struct cmd *cmd;
	cmd = parsepipe(ps, es);
	return cmd;
}

struct cmd* parsepipe(char **ps, char *es) {
	struct cmd *cmd;
	cmd = parseexec(ps, es);
	if(peek(ps, es, "|")) {
		gettoken(ps, es, 0, 0);
		cmd = pipecmd(cmd, parsepipe(ps, es));
	}
	return cmd;
}

struct cmd* parseredirs(struct cmd *cmd, char **ps, char *es) {
	int tok;
	char *q, *eq;
	while(peek(ps, es, "<>")) {
		tok = gettoken(ps, es, 0, 0);
		if(gettoken(ps, es, &q, &eq) != 'a') {
			fprintf(stderr, "missing file for redirection\n");
			exit(-1);
		}
		switch(tok) {
		case '<':
			cmd = redircmd(cmd, mkcopy(q, eq), '<');
			break;
		case '>':
			cmd = redircmd(cmd, mkcopy(q, eq), '>');
			break;
		}
	}
	return cmd;
}

struct cmd* parseexec(char **ps, char *es) {
	char *q, *eq;
	int tok, argc;
	struct execcmd *cmd;
	struct cmd *ret;
	ret = execcmd();
	cmd = (struct execcmd*)ret;
	argc = 0;
	ret = parseredirs(ret, ps, es);
	while(!peek(ps, es, "|")) {
		if((tok=gettoken(ps, es, &q, &eq)) == 0)
			break;
		if(tok != 'a') {
			fprintf(stderr, "syntax error\n");
			exit(-1);
		}
		cmd->argv[argc] = mkcopy(q, eq);
		argc++;
		if(argc >= MAXARGS) {
			fprintf(stderr, "too many args\n");
			exit(-1);
		}
		ret = parseredirs(ret, ps, es);
	}
	cmd->argv[argc] = 0;
	return ret;
}

// создаем копию символов во входном буфере, с s до es
char *mkcopy(char *s, char *es) {
	int n = es - s;
	char *c = malloc(n+1);
	assert(c);
	strncpy(c, s, n);
	c[n] = 0;
	return c;
}

int peek(char **ps, char *es, char *toks) {
	char *s;
	s = *ps;
	while(s < es && strchr(whitespace, *s))
		s++;
	*ps = s;
	return *s && strchr(toks, *s);
}

int gettoken(char **ps, char *es, char **q, char **eq) {
	char *s;
	int ret;
	s = *ps;
	while(s < es && strchr(whitespace, *s))
		s++;
	if(q)
		*q = s;
	ret = *s;
	switch(*s) {
		case 0:
			break;
		case '|':
		case '<':
			s++;
			break;
		case '>':
			s++;
			break;
		default:
			ret = 'a';
			while(s < es && !strchr(whitespace, *s) && !strchr(symbols, 
				*s))
				s++;
			break;
	}
	if(eq)
		*eq = s;
  
	while(s < es && strchr(whitespace, *s))
		s++;
	*ps = s;
	return ret;
}
