#include "shop.h"

// Функция определения числа свободных магазинов
unsigned int nfree_shops(int* shop_list, unsigned int n) {
	unsigned int counter = 0;
	for (int i = 0; i < n; ++i)
		if (0 == shop_list[i])
			++counter;
	return counter;
}

// Функция распределяет покупателей по свободным магазинам
unsigned int distributor(buyer_args_t* arg) {
	pthread_mutex_lock(&(arg->status->qmutex));
	while (1 > nfree_shops((int*)&(arg->status->shop_list), 
		count_shop)) {	//
		sleep(1);// Удерживаем покупателя, пока не освободится магазин
	}
	// Выбор случайного свободного магазина
	unsigned int num = 0;
	do {
		num = rand() % count_shop;
	} while (0 != arg->status->shop_list[num]);
	arg->status->shop_list[num] = (int)arg->id;
	pthread_mutex_unlock(&(arg->status->qmutex));
	return num;
}

// Поток покупателя
void* buyer_handler(void* tmp) {
//	pthread_detach(pthread_self());
	buyer_args_t* args = (buyer_args_t*)tmp;
	// Проверка свободны ли все магазины в данный момент. Если да, то
	//остановка
	pthread_mutex_lock(&(args->status->qmutex));
	unsigned int fs = nfree_shops((int*)&(args->status->shop_list),
		count_shop);
	
	pthread_mutex_unlock(&(args->status->qmutex));
	
	do {
		//Занять магазин
		unsigned int idshop = distributor(args);
		//Дождаться заполнение магазина товаром
		while (args->need > args->status->quantity_of_goods[idshop]) {
			sleep(1);
		}
		//Взять товар
		args->status->quantity_of_goods[idshop] -= args->need;
		//Освободить магазин
		args->status->shop_list[idshop] = 0;
		pthread_mutex_lock(&(args->status->qmutex));
		fs = nfree_shops((int*)&(args->status->shop_list), count_shop);
		pthread_mutex_unlock(&(args->status->qmutex));
		sleep(buyer_sleep); // Засыпает на секунду
	} while (fs != count_shop);
	return NULL;
}
