#include "raw.h"

static uint16_t udp_checksum(uint8_t* udp_hdr, uint8_t* psd_hdr);
static uint16_t ipv4_checksum(uint8_t* ipv4_hdr);

uint8_t* make_fr(uint8_t* dstmac, uint8_t* srcmac, uint8_t* data, uint16_t dlen) {
	struct eth_header* hdr_eth;
	uint8_t* frame;
	uint32_t flength = dlen + sizeof(struct eth_header);

	frame = malloc(flength);
	//if (!frame)
	//	DieWithError("malloc");
	memcpy(frame + sizeof(struct eth_header), data, dlen);

	//Заполняем заголовок
	hdr_eth = (struct eth_header *)frame;
	memcpy(hdr_eth->srcmac, srcmac, 6);
	memcpy(hdr_eth->dstmac, dstmac, 6);
	hdr_eth->proto = 0x08;
	free(data);
	return frame;
}

uint8_t* make_ipv4(uint32_t srcip, uint32_t dstip, uint8_t* data, int dlen) {
	uint8_t* packet;	// Указатель на начало пакета

	uint16_t pack_len = sizeof(struct ipv4_header) + dlen;
	// Выделяем память под пакет и записываем передаваемые данные
	packet = malloc(pack_len);
	//if (!packet)
	//	DieWithError("malloc");
	memcpy(packet + sizeof(struct ipv4_header), data, dlen);

	struct ipv4_header  *ip_hdr;				// Заголовок IP
	ip_hdr = (struct ipv4_header *)packet;
	ip_hdr->version_ihl = 0x5;
	ip_hdr->version_ihl |= 0x40;
	ip_hdr->length = htons(pack_len);
	ip_hdr->ttl = 56;
	ip_hdr->proto = IPPROTO_UDP;
	ip_hdr->srcip = srcip;
	ip_hdr->dstip = dstip;
	ip_hdr->chsum = htons(ipv4_checksum((uint8_t*)ip_hdr));
	
	free(data);
	return packet;
}

static uint16_t ipv4_checksum(uint8_t* ipv4_hdr) {
	int chsum = 0;
	uint16_t length = sizeof(struct ipv4_header);

	for (int i = 0; i < length; i += 2) {
		if (i == 10)
			continue;
		uint16_t loct = (ipv4_hdr[i]);
		uint16_t roct = (ipv4_hdr[i + 1]);
		chsum += (loct << 8) | roct;
	}
	chsum = chsum + (chsum>>16);
	return (uint16_t)~chsum;
}

uint8_t* make_udp(uint32_t srcip, uint32_t dstip, uint16_t sport, uint16_t dport, const uint8_t* data, uint16_t dlen) {
	int pack_len = sizeof(struct udp_header) + dlen;
	struct pseudo_header hdr_psd = {srcip, dstip, 0, IPPROTO_UDP, htons(pack_len)};	// Псевдозаголовок
	uint8_t* packet;	// Данные

	packet = malloc(pack_len);
	//if (!packet)
	//	DieWithError("malloc");
	// Запись данных
	memcpy(packet + sizeof(struct udp_header), data, dlen);
	struct udp_header* udp_hdr;		// Заголовок UDP
	udp_hdr = (struct udp_header *)packet;
	udp_hdr->sport = htons(sport);
	udp_hdr->dport = htons(dport);
	udp_hdr->length = htons(pack_len);
	udp_hdr->chsum = htons(udp_checksum((uint8_t*)udp_hdr, (uint8_t*)&hdr_psd));
	return packet;
}

static uint16_t udp_checksum(uint8_t* udp_hdr, uint8_t* psd_hdr) {
    int chsum = 0;					// Контрольная сумма
    // Псевдозаголовок
    for (int i = 0; i < 12; i += 2){
        uint16_t loct = psd_hdr[i];
        uint16_t roct = psd_hdr[i + 1];
        chsum += (loct<<8)|roct;
    }
	uint16_t length = ntohs(((struct pseudo_header*)psd_hdr)->length);	// Длина пакета
	// Если длина не четная
    if (length % 2 == 1) {
        uint16_t loct = udp_hdr[length - 1];
        chsum += (loct << 8);
        length--;
    }
    // Пакет без учета контрольной суммы
    for (int i = 0; i < length; i += 2){
         if (i == 6)
            continue;
        uint16_t loct = udp_hdr[i];
        uint16_t roct = udp_hdr[i + 1];
        chsum += (loct<<8)|roct;
    }
    chsum = chsum + (chsum>>16);
    return (uint16_t)~chsum;
}
