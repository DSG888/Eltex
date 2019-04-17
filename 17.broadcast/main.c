#include "main.h"

volatile server_t srv = {{0xAAAAAAAA, {0, 0}, {0, 0}, {{0}, {0}}, 0xAAAAAAAA}, (endpoint_t *)&(srv.servertable), {-1}};



int main(int argc, char **argv) {
	if (3 > argc)
		DieWithError("args error");

	struct in_addr myipaddr = {0};
	struct in_addr broadcastaddr = {0};

	if (getipaddr(argv[2], &myipaddr)) {
		fprintf(stderr, "Wrong interface \"%s\"\n", argv[2]);
		return EXIT_FAILURE;
	}
	srv.anonce.myep.ip = myipaddr.s_addr;
	if (getbroadcastaddr(argv[2], &broadcastaddr))
		DieWithError("Wrong interface");
	
	int sock = create_server_socket(srv.anonce.myep.ip);	// Занимаем порт
	

	
	struct srv_handler_s arg0 = {broadcastaddr.s_addr, BCPORT};
	pthread_t thread_msg_bcsender_rc;
	if (pthread_create(&thread_msg_bcsender_rc, NULL, broadcast_handler_srv, &arg0) < 0)
		DieWithError("could not create thread");

//myipaddr.s_addr


	//Server
	
	for (;;) {
		sleep(1);
	}
	
	return EXIT_SUCCESS;
}
