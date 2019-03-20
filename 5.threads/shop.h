#ifndef SHOP_H
#define SHOP_H

#include "main.h"

struct {
	int shop_list[count_shop];				// Покупатели
	int quantity_of_goods[count_shop];		// Количество товара в каждом магазине
	pthread_t buyers_thread[count_buyer];
	pthread_mutex_t qmutex;					// Мьютекс для выбора магазина
} typedef status_t;

struct {
	status_t* status;
	unsigned int id;	// id покупателя [1..unsigned int]
	unsigned int need;	// потребность покупателя
} typedef buyer_args_t;

unsigned int nfree_shops(int* shop_list, unsigned int n);
unsigned int distributor(buyer_args_t* arg);
void* buyer_handler(void* tmp);

#endif
