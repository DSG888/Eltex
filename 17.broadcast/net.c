#include "net.h"

extern server_t srv;

// Определяет IP адрес
int getipaddr(char* ifname, struct in_addr* ip) {
	struct ifreq myip;
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	myip.ifr_addr.sa_family = AF_INET;
	snprintf(myip.ifr_name, IFNAMSIZ, ifname);
	if (ioctl(fd, SIOCGIFADDR, &myip))
		return -1;
	ip->s_addr = ((struct sockaddr_in *)&myip.ifr_addr)->sin_addr.s_addr;
	close(fd);
	return 0;
}

// Определяем BROADCAST адрес
int getbroadcastaddr(char* ifname, struct in_addr* bc) {
	struct ifreq myip;
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	myip.ifr_addr.sa_family = AF_INET;
	snprintf(myip.ifr_name, IFNAMSIZ, ifname);
	if (ioctl(fd, SIOCGIFBRDADDR, &myip))
		return -1;
	bc->s_addr = ((struct sockaddr_in *)&myip.ifr_addr)->sin_addr.s_addr;
	close(fd);
	return 0;
}

// Проверка подключения к узлу
//  Если подключение было, то возвращает -1;
//  Если нет, то номер свободной ячейки;
//  Если все ячейки заняты то -1;
static int checktable(endpoint_t desiredep) {
	uint16_t i = 0;
	for (i = 0; srv.anonce.servertable[i].port && i < MAXSERVERS; ++i)
		if (srv.anonce.servertable[i].ip == desiredep.ip && srv.anonce.servertable[i].port == desiredep.port)
			return -1;
	for (i = 0; srv.anonce.servertable[i].ip && srv.anonce.servertable[i].port && i < MAXSERVERS; ++i);
	if (i == MAXSERVERS - 1 && srv.anonce.servertable[i].ip)
		return -1;
	return i;
}
		
// Подключение к порту сервера, если нужно
static void connecttosrv(endpoint_t ep) {
	//TODO MUTEX?
	int res = checktable(ep);
	if (res == -1) {
		return;
	}
	int sock = client_connect(ep);
	if (sock != -1) {
		srv.anonce.servertable[res].ip = ep.ip;
		srv.anonce.servertable[res].port = ep.port;
		srv.server_socks[res] = sock;
		fprintf(stderr, "Подключился к серверу %s:%d\n", inet_ntoa((struct in_addr){ep.ip}), ep.port);
		// TODO Регистрация дескриптора?
	}
	return;
}
		

void* broadcast_handler_srv(void *args) {
	while (1) {	// Цикл на открытие соккета
	//	fprintf(stderr, "Начал слушать\n");
		int sock_n = 0;
		
		sock_n = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if(sock_n < 0) {
			fprintf(stderr, "cannot open socket\n");
			exit(EXIT_FAILURE);
		}
		int enabledflag = 1;
		if (setsockopt(sock_n, SOL_SOCKET, SO_BROADCAST, &enabledflag, sizeof(enabledflag)) == -1) {
			fprintf(stderr, "setsockopt (SO_BROADCAST)\n");
			exit(EXIT_FAILURE);
		}
		// BROADCAST который мы заслужили
		if (setsockopt(sock_n, SOL_SOCKET, SO_REUSEADDR, &enabledflag, sizeof(enabledflag)) == -1) {
			fprintf(stderr, "setsockopt (SO_REUSEADDR)\n");
			exit(EXIT_FAILURE);
		}
		struct timeval timeV = {0, 300000};
		if (setsockopt(sock_n, SOL_SOCKET, SO_RCVTIMEO, &timeV, sizeof(timeV)) == -1) {
			fprintf(stderr, "setsockopt (SO_RCVTIMEO)\n");
			exit(EXIT_FAILURE);
		}
		struct sockaddr_in servAddr;		// Отправитель
		memset(&servAddr, 0, sizeof(servAddr));
		servAddr.sin_family = AF_INET;
		servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servAddr.sin_port = htons(((struct srv_handler_s*)args)->port);
		struct sockaddr_in broadcastAddr;	// Назначение
		memset(&broadcastAddr, 0, sizeof(broadcastAddr));
		broadcastAddr.sin_family = AF_INET;
		broadcastAddr.sin_addr.s_addr = (((struct srv_handler_s*)args)->ip);
		broadcastAddr.sin_port = htons(((struct srv_handler_s*)args)->port);
		
		if (0 > bind(sock_n, (struct sockaddr*)&servAddr, sizeof(servAddr))) {
			fprintf(stderr, "cannot bind port number %d\n", ((struct srv_handler_s*)args)->port);
			exit(EXIT_FAILURE);
		}
		while(1) { // Цикл на случай получения мусора на порт
			broadcastanonce_t buf = {0x0, {0, 0}, {0, 0}, {{0}, {0}}, 0x0};
			int n = 0;
			if ((n = recvfrom(sock_n, &buf, sizeof(buf), 0, NULL, NULL)) < 0) {
				//fprintf(stderr, "Вермя вышело\n");
				static int a = 0;
				if (++a < 5) {
					fprintf(stderr, "Пропуск\n");
					break;
				}
				if (!srv.anonce.masterknot.ip) {
					fprintf(stderr, "Тогда я мастерНода\n");
					srv.anonce.masterknot.ip = srv.anonce.myep.ip;
					srv.anonce.masterknot.port = srv.anonce.myep.port;
				}
				if (sendto(sock_n, &srv.anonce, sizeof(srv.anonce), 0, (struct sockaddr_in*)&broadcastAddr, sizeof(broadcastAddr)) != sizeof(srv.anonce)) {
					fprintf(stderr, "sendto() sent a different number of bytes than expected\n");
				}
				break;
			}
			if (n != sizeof(broadcastanonce_t) || buf.magic1 != MAGICNUM || buf.magic2 != MAGICNUM) {
				fprintf(stderr, "Получен мусор %d\n", n);
				break;
			}
			if (buf.myep.ip == srv.anonce.myep.ip && buf.myep.port == srv.anonce.myep.port) {
				fprintf(stderr, "Получен свой же пакет\n");
				break;
			}
			fprintf(stderr, "Получен анонс: от %s:%d с мастер нодой ", inet_ntoa((struct in_addr){buf.myep.ip}), buf.myep.port);
			fprintf(stderr, "%s:%d\n", inet_ntoa((struct in_addr){buf.masterknot.ip}), buf.masterknot.port);
			//connecttosrv(buf.masterknot);
			connecttosrv(buf.myep);
			
			for (uint16_t i = 0; buf.servertable[i].port && i < MAXSERVERS; ++i) {
				fprintf(stderr, " %s:%d\n", inet_ntoa((struct in_addr){buf.servertable[i].ip}), buf.servertable[i].port);
				// Проверка подключен ли данный сервер к узлам из списка
				connecttosrv(buf.myep);
				
			}
			
			srv.anonce.masterknot.ip = buf.masterknot.ip;
			srv.anonce.masterknot.port = buf.masterknot.port;
		}
		close(sock_n);
	}
	return 0;
}

int create_server_socket(uint32_t ip) {
	while (1) {
		for (uint16_t port = PORTA; port < (uint16_t)(PORTA + NUMPORTS); ++port) {
			int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (-1 == sock) {
				fprintf(stderr, "Could not create socket\n");
				exit(EXIT_FAILURE);
			}
			struct sockaddr_in server;
			memset(&server, 0, sizeof(server));
			server.sin_family = AF_INET;
			server.sin_addr.s_addr = ip;
			server.sin_port = htons(port);
			if (bind(sock, (struct sockaddr*)&server , sizeof(server)) < 0) {
		//		fprintf(stderr, "bind failed\n");
				continue;
			}
			if (listen(sock, MAXPENDING) < 0) {
				fprintf(stderr, "listen failed\n");
				exit(EXIT_FAILURE);
			}
			srv.anonce.myep.port = port;
			return sock;
		}
		fprintf(stderr, "Не удалось занять порт из диапазона: [%d:%d]\n", PORTA, (uint16_t)(PORTA + NUMPORTS));
		sleep(1);
	}
}

int client_connect(endpoint_t ep) {
	int sock;
	struct sockaddr_in client;
	sock = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
	if (sock == -1) {
		fprintf(stderr, "Could not create socket\n");
		exit(EXIT_FAILURE);
	}
	memset(&client, 0, sizeof(client));
	client.sin_addr.s_addr = ep.ip;
	client.sin_family = AF_INET;
	client.sin_port = htons(ep.port);
	if (connect(sock, (struct sockaddr*)&client, sizeof(client)) < 0) {
		return -1;
	}
	return sock;
}
