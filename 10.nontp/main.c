#include "main.h"

int main(int argc, char * argv[]) {
	// Тут создается очень случайное зерно
	//srand(time(0) + getpid());
	int proto = 0, mode = 0;
	unsigned int port = DEFAULT_PORT;
	struct in_addr out_ip = {0};
	char* hostname = DEFAULT_IP;
	if (2 > argc || argc > 4)
		argerror:
		DieWithError("args error. example:\n ./sock ct <ip> <port>\n ./sock st <ip> <port>\n ./sock su <ip> <port>\n ./sock cu <ip> <port>\n");
	// Просто парсинг параметров
	if (strlen(argv[1]) != 2 || !(mode = (!strchr(server_key, argv[1][0])?0:SERVER_MODE) | (!strchr(client_key, argv[1][0])?0:CLIET_MODE)) || !(proto = (((strchr(tcp_key, argv[1][1]))?TCP_PROTO:0) | ((strchr(udp_key, argv[1][1]))?UDP_PROTO:0))))
		goto argerror;
	if (argc >= 3)
		hostname = argv[2];
	if (argc == 4 && ((port = atoi(argv[3])) > 65535))
		goto argerror;
	fprintf(stdout, "Режим %s с %s протоколом\n", mode==SERVER_MODE?"сервера":"клиента", proto==TCP_PROTO?"TCP":"UDP");
	// Резолв хоста через DNS
	if (1) {
		struct hostent* he;
		if ((he = gethostbyname(hostname)) == NULL)
			DieWithError("DNS сломан\n");
		struct in_addr **addr_list = (struct in_addr **) he->h_addr_list;
		for(int i = 0; addr_list[i] != NULL; ++i) {
			out_ip = *addr_list[i];
			break;
		}
	}
	fprintf(stdout, "Используется хост '%s' -> %s:%d\n", hostname, inet_ntoa(out_ip), port);
	signal(SIGPIPE, SIG_IGN);	// На случай неудачной передачи
	// Запуск отсоединенных потоков
	pthread_t thread_handler;
	struct main_handler_args_s srv_args = {out_ip.s_addr, port};
	if (mode == SERVER_MODE) {
		if (pthread_create(&thread_handler, NULL, proto==TCP_PROTO?server_handler_tcp:server_handler_udp, &srv_args) < 0)
			DieWithError("thread_create\n");
	}
	else {
		if (pthread_create(&thread_handler, NULL, proto==TCP_PROTO?client_handler_tcp:client_handler_udp, &srv_args) < 0)
			DieWithError("thread_create\n");
	}
	// Ну основной поток ничего делать не будет
	while (1) {
		sleep(1);
	}
	
	return EXIT_SUCCESS;
}
