#ifndef RAW_H
#define RAW_H

#include <stdint.h>		//uint8_t uint16_t uint32_t
#include <stdlib.h>		//malloc

#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <string.h>

#define BUFSIZE 1500
#define DEFAULT_LEN 56

//	                            ICMP пакет
//	|<---------------------------4 байта--------------------------->|
//	|    Тип:8     |    Код:8     |      Контрольная сумма:16       |
//	|     Идентификатор:16     |     Номер последовательности:16    |
//	|Время жизни пакета:8|Протокол:8|Контрольная сумма заголовка:16 |
struct icmphdr {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    union{
		struct{
			uint16_t id;
			uint16_t sequence;
		}echo;
		uint32_t gateway;
		struct{
			uint16_t unsed;
			uint16_t mtu;
		}frag;
    }un;
    uint8_t data[0];
#define icmp_id un.echo.id
#define icmp_seq un.echo.sequence
};
#define ICMP_HSIZE sizeof(struct icmphdr)

struct iphdr {
    uint8_t hlen:4, ver:4;
    uint8_t tos;
    uint16_t tot_len;
    uint16_t id;
    uint16_t frag_off;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t check;
    uint32_t saddr;
    uint32_t daddr;
};

#endif
