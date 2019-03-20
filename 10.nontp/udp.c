#include "udp.h"

int create_server_udp_socket(unsigned int ip, int port, int binding) {
	int sock = -1;
	struct sockaddr_in echoServAddr;
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("socket() failed");
	memset(&echoServAddr, 0, sizeof(echoServAddr));
	// Таймаут в 2 секунды на передачу
	struct timeval timeV = {2, 0};
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeV, sizeof(timeV)) == -1) {
		fprintf(stderr, "setsockopt (SO_RCVTIMEO)\n");
		exit(EXIT_FAILURE);
	}
	if (binding) {
		memset(&echoServAddr, 0, sizeof(echoServAddr));
		echoServAddr.sin_family = AF_INET;
		echoServAddr.sin_addr.s_addr = binding?htonl(INADDR_ANY):ip;
		echoServAddr.sin_port = htons(port);
		if (bind(sock, (struct sockaddr*)&echoServAddr, sizeof(echoServAddr)) < 0)
			DieWithError("bind() failed");
	}
	return sock;
}

void* client_handler_udp(void *threadArgs) {
	pthread_detach(pthread_self());		// Guarantees that thread resources are deallocated upon return
	
	struct sockaddr_in echoServAddr;
	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = (((struct main_handler_args_s*)threadArgs)->ip);
	echoServAddr.sin_port = htons(((struct main_handler_args_s*)threadArgs)->port);
	
	fprintf(stdout, "Основной поток клиента запущен\n");
	int sock = create_server_udp_socket(((struct main_handler_args_s*)threadArgs)->ip, ((struct main_handler_args_s*)threadArgs)->port, 0);
	struct sockaddr_in fromAddr;
	unsigned int fromSize;
	while (1) {
		// Отправка запроса
		fprintf(stdout, "  Отправка запроса");
		const unsigned int data = MAGIC_NUM;
		if ((sendto(sock, &data, sizeof(data), 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) != sizeof(data))) {
			fprintf(stdout, " не удалась\n");
			sleep(3);
			continue;
		}
		fromSize = sizeof(fromAddr);
		// Получение строки
		char buf[256] = {'\0'};
		if ((recvfrom(sock, &buf, sizeof(buf), 0, (struct sockaddr*) &fromAddr, &fromSize)) != sizeof(buf)) {
			//DieWithError("recvfrom() failed");
			fprintf(stdout, "не удалась\n");
			sleep(3);
			continue;
		}
		fprintf(stdout, ". Получена '%s'\n", buf);
		// Проверка отправителя
		if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
			fprintf(stderr,"Warn: получил пакет из неизвестного источника\n");
			sleep(5);
			continue;
		}
		sleep(1);
		continue;
	}
	return NULL;
}

void* server_handler_udp(void *threadArgs) {
	pthread_detach(pthread_self());		// Guarantees that thread resources are deallocated upon return
	
	fprintf(stdout, "Основной поток сервера запущен\n");
	int sock = create_server_udp_socket(((struct main_handler_args_s*)threadArgs)->ip, ((struct main_handler_args_s*)threadArgs)->port, 1);
	while (1) {
		unsigned int data = 0;
		struct sockaddr_in echoClntAddr;
		unsigned int cliAddrLen = sizeof(echoClntAddr);

		// Получение запроса от клиента (и его адреса)
		fprintf(stdout, "  Получение запроса");
		if ((recvfrom(sock, &data, sizeof(data), 0, (struct sockaddr*)&echoClntAddr, &cliAddrLen)) != sizeof(data) || MAGIC_NUM != data) {
			fprintf(stdout, " не удалось\n");
			continue;
		}
		// Получение системной даты
		long int ttime = time(NULL);
		char buf[256] = {'\0'};
		sprintf(buf, "%s", ctime(&ttime));
		
		// Отправка строки
		fprintf(stdout, ". Отправка даты %s", buf);
		if (sendto(sock, &buf, sizeof(buf), 0, (struct sockaddr*)&echoClntAddr, sizeof(echoClntAddr)) != sizeof(buf)) {
			fprintf(stdout, " не удалась\n");
			continue;
		}
		fprintf(stdout, " [OK]\n");
		sleep(1);
	}
	return NULL;
}
