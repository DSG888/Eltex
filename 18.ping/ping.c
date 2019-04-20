#include "ping.h"



uint16_t checksum(uint8_t *buf,int len) {
	uint32_t sum=0;
	uint16_t *cbuf;

	cbuf=(uint16_t *)buf;

	while(len>1){
		sum+=*cbuf++;
		len-=2;
	}

	if(len)
		sum+=*(uint8_t *)cbuf;

	sum=(sum>>16)+(sum & 0xffff);
	sum+=(sum>>16);

	return ~sum;
}
