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

//	                            Ethernet кадр
//	|<---------------------------6 байт---------------------------->|
//	|                   IP адрес источника:48                       |
//	|                   IP адрес назначения:48                      |
//	|   Тип инкапсулированного протокала:16     |      Данные~      |
struct eth_header {
    uint8_t dstmac[6];    // MAC адрес получателя    
    uint8_t srcmac[6];    // MAC адрес отправителя
    uint16_t proto;      // Тип инкапсулированного протокала
};

//	                            IPv4 пакет
//	|<---------------------------4 байта--------------------------->|
//	| Версия:4  | Длина заголовка:4 | Сервис:6  | Длина пакета:16   |
//	| Идентификатор:16  | Флаги:3   |   Смещение фрагмента:13       |
//	|Время жизни пакета:8|Протокол:8|Контрольная сумма заголовка:16 |
//	|                   IP адрес источника:32                       |
//	|                   IP адрес назначения:32                      |
//	|           Опции:~             ||          Padding             |
struct ipv4_header {
	uint8_t version_ihl;
	uint8_t service_tpye;
	uint16_t length;
	uint16_t identification;
	uint16_t flag_offset;
	uint8_t ttl;
	uint8_t proto;
	uint16_t chsum;
	uint32_t srcip;
	uint32_t dstip;
	//Options?
};

//	                    UDP сегмент
//	|<-------------------4 байта------------------->|
//	|   Порт источника:16   |   Порт назанчения:16  |
//	|    Длина пакета:16    | Контрольная сумма:16  |
struct udp_header {
	uint16_t sport;
	uint16_t dport;
	uint16_t length;
	uint16_t chsum;
};

//	                Псевдозаголовок
//	|<-------------------4 байта------------------->|
//	|               IP источника:32                 |
//	|               IP назначения:32                |
//	|   Нули:8  |   Протокол:6  | Длина сегмента:16 |
struct pseudo_header {
	uint32_t sip;
	uint32_t dip;
	uint8_t zeros;
	uint8_t proto;
	uint16_t length;
};

uint8_t* make_udp(uint32_t srcip, uint32_t dstip, uint16_t sport, uint16_t dport, const uint8_t* data, uint16_t dlen);

uint8_t* make_ipv4(uint32_t srcip, uint32_t dstip, uint8_t* data, int dlen);


uint8_t* make_fr(uint8_t* dstmac, uint8_t* srcmac, uint8_t* data, uint16_t dlen);

#endif
