#ifndef MAIN_H
#define MAIN_H

#define count_shop 5		// Число магазинов
#define count_buyer 3		// Число покупателей
#define buyers_need 1000	// Потребность
#define buyer_sleep 1		// Сон покупателя
#define variance 100		// Разброс
#define add 80				// Число товара, которое кладет погрузчик

#include <stdio.h>
#include <errno.h>	//EXIT_SUCCESS
//#include <stdint.h>
#include <pthread.h>	//pthread_t pthread_mutex_t
//#include <signal.h>
#include <stdlib.h>
#include <unistd.h>	//sleep

#include "shop.h"


#define cl_green "\033[32m"
#define cl_blue "\033[34m"
#define cl_red "\033[31m"
#define cl_normal "\033[0m"
#define ch_clear "\033c"



#endif
