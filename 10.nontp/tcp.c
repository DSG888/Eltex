#include "tcp.h"

int create_server_tcp_socket(unsigned int ip, int port) {
	struct sockaddr_in server;
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (-1 == sock) {
		fprintf(stderr, "Could not create socket\n");
		exit(EXIT_FAILURE);
	}
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = ip;
	server.sin_port = htons(port);
	if (bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
		fprintf(stderr, "bind failed\n");
		exit(EXIT_FAILURE);
	}
	if (listen(sock, MAXPENDING) < 0) {
		fprintf(stderr, "listen failed\n");
		exit(EXIT_FAILURE);
	}
	return sock;
}

int accept_tcp_connection(int servSock) {
	int clntSock;
	struct sockaddr_in echoClntAddr;
	unsigned int clntLen = sizeof(echoClntAddr);
	if ((clntSock = accept(servSock, (struct sockaddr*) &echoClntAddr, &clntLen)) < 0)
		DieWithError("accept() failed");
	fprintf(stdout, "Подключен клиент с ip %s\n", inet_ntoa(echoClntAddr.sin_addr));
	return clntSock;
}

int client_tcp_connect(unsigned int ip, int port) {
	int sock;
	struct sockaddr_in client;
	sock = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
	if (sock == -1) {
		fprintf(stderr, "Could not create socket\n");
		exit(EXIT_FAILURE);
	}
	memset(&client, 0, sizeof(client));
	client.sin_addr.s_addr = ip;
	client.sin_family = AF_INET;
	client.sin_port = htons(port);
	if (connect(sock, (struct sockaddr*)&client , sizeof(client)) < 0) {
		return -1;
	}
	return sock;
}

void* server_worker_tcp(void *threadArgs) {
	/* Guarantees that thread resources are deallocated upon return */
	pthread_detach(pthread_self()); 
	
	fprintf(stdout, " Подключен клиент №%d\n", ((struct connect_node_s*)threadArgs)->num);
	int sock = ((struct connect_node_s*)threadArgs)->sock;

	// Получение системной даты
	long int ttime = time(NULL);
	char buf[256] = {'\0'};
	sprintf(buf, "%s", ctime(&ttime));
	fprintf(stdout, "  Передача даты: %s", buf);
	// Передача строки
	if ((send(sock, buf, sizeof(buf), 0)) != sizeof(buf))
		fprintf(stdout, " [FAIL]\n");
	else
		fprintf(stdout, " [OK]\n");
	close(sock);
	return NULL;
}

void* server_handler_tcp(void *threadArgs) {
	pthread_detach(pthread_self());		// Guarantees that thread resources are deallocated upon return

	fprintf(stdout, "Основной поток сервера запущен\n");
	int client_count = 0;
	int sock = create_server_tcp_socket(((struct main_handler_args_s*)threadArgs)->ip, ((struct main_handler_args_s*)threadArgs)->port);
	connect_node_t* newconnect = NULL;
	while (1) {
		newconnect = malloc(sizeof(connect_node_t));	//TODO alloc?
		if (!newconnect)
			DieWithError("memerror");
		newconnect->num = client_count++;
		fprintf(stdout, " Жду подключения\n");
		newconnect->sock = accept_tcp_connection(sock);
		if ((newconnect->client_threadID = pthread_create(&newconnect->client_threadID, NULL, server_worker_tcp, (void*)newconnect)) < 0) {
			DieWithError("\npthread_create() failed");
		}
	}
	return NULL;
}

void* client_handler_tcp(void *threadArgs) {
	pthread_detach(pthread_self());		// Guarantees that thread resources are deallocated upon return
	
	fprintf(stdout, "Основной поток клиента запущен\n");
	struct in_addr ip = {((struct main_handler_args_s*)threadArgs)->ip};
	
	while (1) {
		fprintf(stdout, " Подключение к %s:%d\t", inet_ntoa(ip), ((struct main_handler_args_s*)threadArgs)->port);
		int sock = client_tcp_connect(((struct main_handler_args_s*)threadArgs)->ip, ((struct main_handler_args_s*)threadArgs)->port);
		if (0 > sock) {
			fprintf(stdout, "[FAIL]\n");
			sleep(3);
			continue;
		}
		fprintf(stdout, "[ОК]\n");
		// Получение строки
		char buf[256] = {'\0'};
		if ((recv(sock, buf, sizeof(buf), 0)) !=  sizeof(buf))
			fprintf(stdout, " [FAIL]\n");
		else
			fprintf(stdout, "  Получена строка: %s\nОтключился\n", buf);
		sleep(1);
		close(sock);
	}
	return NULL;
}
