#include "shmalloc.h"

// Идентификаторы для очереди сообщений, семафора, разделяемой памяти
int qid, sid, mid;

// Массив пользователей
int *users;

// Семафор
union semun arg;
struct sembuf sb;

pid_t procespid;

void main_handler(int flagmsq, int flagshm, int flagsem)
{
	// Ключи для очереди сообщений, семафора, разделяемой памяти
	key_t qkey, skey, mkey;

	// Структура получения и отправки сообщений
	struct my_msgbufer buf;
	struct my_msgbufer bufrcv;

	uid_t userid;
	struct passwd *pwd;
	char newpid[6] = {0};

	int flag = 0, ctr = 0;

	pid_t child;

	// Создание всех необходимых устройств IPC
	if (flagmsq == 0) {
		// Получение ключа для очереди
		if((qkey = ftok("/etc/fstab", 1)) == -1) {
			perror("msq ftok");
			exit(2);
		};

		// Подключение или создание очереди сообщений
		if ((qid = msgget(qkey, 0644 | IPC_CREAT)) == -1) {
			perror("msgget");
			exit(2);
		};
	}

	if (flagsem == 0) {
		// Получение ключа для семафора
		if((skey = ftok("/etc/fstab", 2)) == -1) {
			perror("sem ftok");
			exit(2);
		};

		// Создание семафора
		if ((sid = semget(skey, 1, 0666 | IPC_EXCL | IPC_CREAT)) == -1) {
			if ((sid = semget(skey, 1, 0666 |IPC_CREAT)) == -1) {
				perror("semget");
				exit(2);
			}
		}
		else {
			arg.val = 1;
			if (semctl(sid, 0, SETVAL, arg) == -1) {
				perror("semctl");
				exit(2);
			}
		}
	}

	// Установка семафора
	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = 0;

	if (flagshm == 0) {
		// Получение ключа для общей памяти
		if((mkey = ftok("/etc/fstab", 3)) == -1) {
			perror("mem ftok");
			exit(2);
		};

		// Создание или подключение общей памяти
		if ((mid = shmget(mkey, sizeof(int[MAX_USERS]), 0644 | IPC_EXCL | IPC_CREAT)) == -1) {
			if ((mid = shmget(mkey, sizeof(int[MAX_USERS]), 0644 | IPC_CREAT)) == -1) {
				perror("shmget");
				exit(2);
			}
		}
		else
			ctr = 1;
	}

	// Разметка сегмента
	users = (int*)shmat(mid, 0, 0);

	// Ининциализация массива
	if (ctr == 1)
		for (int i = 0 ; i < MAX_USERS; ++i)
			users[i] = 0;
	procespid = getpid();

	if (semop(sid, &sb, 1) == -1) {
		perror("semop");
		exit(2);
	}

	for (int i = 0; i < MAX_USERS; ++i)
		if (users[i] == 0 && flag == 0) {
			users[i] = procespid;
			flag = 1;
		}

	sb.sem_op = 1;
	if (semop(sid, &sb, 1) == -1) {
		perror("semop");
		exit(2);
	}

	if (flag == 0) {
		printf("Чат переполнен\n");
		exit(0);
	}

	// Заполнение информации о юзере
	userid = getuid();
	pwd = getpwuid(userid);
	strcpy(buf.usrname, pwd -> pw_name);
	sprintf(newpid, " %d", getpid());
	strcat(buf.usrname, newpid);
	strcat(buf.usrname, "\0");

	// Вместо потоков используем процессы
	if ((child = fork()) < 0) {
		perror("fork");
		exit(2);
	}
	else if (child == 0) {
		while (1) {
			// Получение сообщения
			if (msgrcv(qid, (struct msgbuf *)&bufrcv, sizeof(bufrcv), procespid, 0) == -1) {
				perror("msgrcv");
				exit(2);
			}
			printf("%s: %s\n", bufrcv.usrname, bufrcv.mtext);
		}
	}
	else {
		while(1) {
			// Отправка сообщения
			if (fgets(buf.mtext, MAXLINE, stdin) != NULL) {
				if (buf.mtext[0] == '\n')
					continue;
				else
					// Если юзер ввел :q, то ему надоело
					if (buf.mtext[0] == ':' && buf.mtext[1] == 'q')
						break;

				sb.sem_op = -1;
				if (semop(sid, &sb, 1) == -1) {
					perror("semop");
					exit(2);
				}

				for (int i = 0 ; i < MAX_USERS ; ++i) {
					if (users[i] == 0)
						continue;
					else {
						buf.mtype = users[i];
						if (msgsnd(qid, (struct msgbuf *)&buf, sizeof(buf), 0) == -1)
							perror("msgsnd");
					}
				}

				sb.sem_op = 1;
				if (semop(sid, &sb, 1) == -1) {
					perror("semop");
					exit(2);
				}
			}
			else {
				printf("Выход по CTRL+D\n");
				cleanup();
				break;
			}
		}
	}
}

void cleanup()
{
	int ctr = 0;

	// Освобождение ресурсов
	sb.sem_op = -1;
	if (semop(sid, &sb, 1) == -1) {
		perror("semop");
		exit(2);
	}

	for(int i = 0 ; i < MAX_USERS ; ++i) {
		if (users[i] == procespid)
			users[i] = 0;
	}

	// Проверка необходимости удаления устройств IPC
	for (int i = 0; i < MAX_USERS; ++i) {
		if (users[i])
			ctr = 1;
	}

	sb.sem_op = 1;
	if (semop(sid, &sb, 1) == -1) {
		perror("semop");
		exit(2);
	}

	if (ctr == 0) {
		// Удаление семафора
		if (semctl(sid, 0, IPC_RMID, arg) == -1) {
			perror("semctl");
			exit(2);
		}

		// Удаление очереди
		if (msgctl(qid, IPC_RMID, NULL) == -1) {
			perror("msgctl");
			exit(2);
		}

		// Удаление общей памяти
		if (shmctl(mid, IPC_RMID, NULL) == -1) {
			perror("shmctl");
			exit(2);
		}
	}
	else
		// Отсоединение от общей памяти
		shmdt((void *)users);
}

void signalc(int s)
{
	printf("\nKeyboard interupt\n");
	cleanup();
	exit(0);
}
