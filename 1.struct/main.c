#include <stdio.h>		//printf
#include <stdlib.h>		//EXIT_SUCCESS

//                  Example Internet Datagram Header
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |Version|  IHL  |Type of Service|          Total Length         |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |         Identification        |Flags|      Fragment Offset    |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |  Time to Live |    Protocol   |         Header Checksum       |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                       Source Address                          |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                    Destination Address                        |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                    Options                    |    Padding    |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct internet_datagram_header_s {
	unsigned a:4;	unsigned ihl:4;	unsigned tos:8;	unsigned total_length:16;
	unsigned identification:16;	unsigned flags:3;	unsigned fragment_offset:13;
	unsigned ttl:8;	unsigned protocol:8;	unsigned header_checksum:16;
	unsigned source_address:32;
	unsigned destination_address:32;
    //Options?
    unsigned trash:10;
} typedef internet_datagram_header_t;

// I
struct internet_datagram_header_s2 {
	unsigned a:4;	unsigned ihl:4;	unsigned tos:8;	unsigned total_length:16;
	unsigned identification:16;	unsigned flags:3;	unsigned fragment_offset:13;
	unsigned ttl:8;	unsigned protocol:8;	unsigned header_checksum:16;
	unsigned source_address:32;
	unsigned destination_address:32;
    //Options?
    unsigned trash:10;
}__attribute__((packed)) typedef internet_datagram_header_t2;

// II
#pragma pack(push, 1)
struct internet_datagram_header_s3 {
	unsigned a:4;	unsigned ihl:4;	unsigned tos:8;	unsigned total_length:16;
	unsigned identification:16;	unsigned flags:3;	unsigned fragment_offset:13;
	unsigned ttl:8;	unsigned protocol:8;	unsigned header_checksum:16;
	unsigned source_address:32;
	unsigned destination_address:32;
    //Options?
    unsigned trash:10;
} typedef internet_datagram_header_t3;
#pragma pack(pop)

// III
// Ключ -fpack-struct при компиляции

int main(int argc, char **argv) {
	printf("Размер упакованной структуры: %d\n", sizeof(internet_datagram_header_t));
	printf("Размер не упакованной структуры 1: %d\n", sizeof(internet_datagram_header_t2));
	printf("Размер не упакованной структуры 2: %d\n", sizeof(internet_datagram_header_t3));
	return EXIT_SUCCESS;
}
