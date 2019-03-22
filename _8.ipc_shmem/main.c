#include "main.h"

int main(int argc, char **argv)
{
	// Обработка прерываний
	sigset_t iset;
	struct sigaction act;
	sigemptyset(&iset);
	act.sa_handler = &signalc;
	act.sa_mask = iset;
	act.sa_flags = 0;
	sigaction(SIGINT, &act, NULL);

	int flagmsq = 0, flagshm = 0, flagsem = 0;
	if (argc > 1) {
		int opt;
		while ((opt = getopt(argc, argv, "m:q:s")) != -1) {
			switch (opt) {
			case 'm':
				flagshm = 1;
				mid = atoi(optarg);
				break;
			case 'q':
				flagmsq = 1;
				qid = atoi(optarg);
				break;
			case 's':
				flagsem = 1;
				sid = atoi(optarg);
				break;
			default:
				fprintf(stderr, "Usage: %s [-m shmid] [-q msqid] [-s semid]\n", argv[0]);
				exit(1);
			}
		}
	}
	main_handler(flagmsq, flagshm, flagsem);
	cleanup();
	return 0;
}
