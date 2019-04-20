#include "raw.h"
#include "ping.h"

static int datalen = DEFAULT_LEN;

#define IP_HSIZE sizeof(struct iphdr)

static struct itimerval val_alarm={.it_interval.tv_sec = 1, .it_interval.tv_usec=0, .it_value.tv_sec=0, .it_value.tv_usec=1};

int main(int argc, char **argv) {
	struct hostent *host;
	int on =1;

	if (argc < 2) {
		printf("Usage: %s hostname\n",argv[0]);
		exit(1);
	}

	if ((host = gethostbyname(argv[1])) == NULL) {
		perror("can not understand the host name");
		exit(1);
	}

	hostname=argv[1];

	memset(&dest, 0, sizeof dest);
	dest.sin_family = PF_INET;
	dest.sin_port = ntohs(0);
	dest.sin_addr =*(struct in_addr *)host->h_addr_list[0];

	if((sockfd = socket(PF_INET,SOCK_RAW,IPPROTO_ICMP)) < 0) {
		perror("raw socket created error");
		exit(1);
	}

	setsockopt(sockfd,IPPROTO_IP,IP_HDRINCL,&on,sizeof(on));

	setuid(getuid());
	pid=getpid();

	set_sighandler();
	printf("Ping %s(%s): %d bytes data in ICMP packets.\n", argv[1], inet_ntoa(dest.sin_addr), datalen);

	if((setitimer(ITIMER_REAL,&val_alarm,NULL))==-1)
		bail("setitimer fails.");

	recv_reply();
	return 0;
}

void send_ping(void) {
	struct iphdr* ip_hdr;
	struct icmphdr* icmp_hdr;
	ip_hdr = (struct iphdr *)sendbuf;
	ip_hdr->hlen = sizeof(struct iphdr) >> 2;
	ip_hdr->ver = 4;	//IPv4
	ip_hdr->tos = 0;
	ip_hdr->tot_len = IP_HSIZE + ICMP_HSIZE + datalen;
	ip_hdr->id = 0;
	ip_hdr->frag_off = 0;
	ip_hdr->protocol = IPPROTO_ICMP;
	ip_hdr->ttl = 255;
	ip_hdr->daddr = dest.sin_addr.s_addr;
	int len1 = ip_hdr->hlen << 2;
	icmp_hdr=(struct icmphdr *)(sendbuf + len1);
	icmp_hdr->type = 8;
	icmp_hdr->code = 0;
	icmp_hdr->icmp_id = pid;
	icmp_hdr->icmp_seq = nsent++;
	memset(icmp_hdr->data, 0xff, datalen); 

	gettimeofday((struct timeval *)icmp_hdr->data,NULL);

	int len = ip_hdr->tot_len;
	icmp_hdr->checksum = 0;
	icmp_hdr->checksum = checksum((uint8_t*)icmp_hdr,len);

	sendto(sockfd, sendbuf, len, 0, (struct sockaddr *)&dest, sizeof(dest));
}

void recv_reply() {
	int n = 0, nrecv = 0, len = 0;
	len = sizeof(struct sockaddr);
	while (nrecv < 4){
		if ((n = recvfrom(sockfd, recvbuf, sizeof (recvbuf), 0, (struct sockaddr *)&from, (socklen_t*)&len))<0) {
			if (errno==EINTR)
				continue;
		}
		gettimeofday(&recvtime,NULL);
		if(handle_pkt())
			continue;
		nrecv++;
	}
	get_statistics(nsent, nrecv);
}

int handle_pkt() {
	struct iphdr *ip;
	struct icmphdr *icmp;

	int ip_hlen;
	uint16_t ip_datalen;
	double rtt;
	struct timeval *sendtime;

	ip=(struct iphdr *)recvbuf;
	ip_hlen=ip->hlen << 2;
	ip_datalen=ntohs(ip->tot_len)-ip_hlen;
	icmp = (struct icmphdr*)(recvbuf+ip_hlen);

	if (checksum((uint8_t*)icmp,ip_datalen))
		return -1;

	if (icmp->icmp_id != pid)
		return -1;

	sendtime=(struct timeval *)icmp->data;
	rtt = ((&recvtime)->tv_sec-sendtime->tv_sec) * 1000 + ((&recvtime)->tv_usec-sendtime->tv_usec) / 1000.0;
	printf("%d bytes from %s:icmp_seq=%u ttl=%d rtt=%.3f ms\n", ip_datalen, inet_ntoa(from.sin_addr), icmp->icmp_seq, ip->ttl, rtt);

	return 0;
}

void set_sighandler() {
	act_alarm.sa_handler=alarm_handler;
	if(sigaction(SIGALRM,&act_alarm,NULL) == -1)
		bail("SIGALRM handler setting fails.");

	act_int.sa_handler=int_handler;
	if(sigaction(SIGINT, &act_int, NULL) == -1)
		bail("SIGALRM handler setting fails.");
}

void get_statistics(int nsent, int nrecv) {
	printf("--- %s ping statistics ---\n", inet_ntoa(dest.sin_addr));
	printf("%d packets transmitted, %d received, %0.0f%% ""packet loss\n", nsent, nrecv,1.0 * (nsent - nrecv) / nsent * 100);
}

void bail(const char * on_what) {
	fputs(strerror(errno), stderr);
	fputs(":",stderr);
	fputs(on_what,stderr);
	fputc('\n',stderr);
	exit(1);
}

void int_handler(int sig) {
	get_statistics(nsent,nrecv);
	close(sockfd);
	exit(1);
}

void alarm_handler(int signo) {
	send_ping();
}
