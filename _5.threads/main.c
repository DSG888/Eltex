/*
	Нужно написать многопоточное приложение. У вас есть 5 магазинов.
В них допустим, рендомно генерируете какое-то количество товара в 
диапазоне до 1000. Ну просто рендомно - это имеется в виду с 
отклонением в 100. Это может быть от 900 и до 1100. Есть 3 потока 
покупателей. Когда покупатель заходит в магазин, только он может 
находиться в этом магазине. Другие могут в другие ходить. 
Соответственно он заходит и покупает какую то часть товара. У него 
есть своя потребность. Потребность задается когда ты создаешь поток 
покупателя. То есть тоже рендомное число, больше чем есть в магазине. 
Когда он купил товар, он засыпает на секунду на пример. Через секунду 
просыпается и снова лезет в случайный магазин, лезет в него пока он 
свою потребность не искупит. Есть еще один поток - погрузчик. У 
которого задача обратная. Он подходит в магазин и нагружает. И вся эта 
схема умирает в тот момент, когда все 3 потока насытятся.
*/

#include "main.h"

int main(int argc, char * argv[]) {
	status_t status;
	buyer_args_t args[count_buyer];
	for (int i = 0; i < count_shop; ++i) {
		status.shop_list[i] = 0;
		status.quantity_of_goods[i] = 0;
	}
	pthread_mutex_init(&(status.qmutex), NULL);
	for (int i = 0; i < count_buyer; ++i) {
		args[i].id = i + 1;
		args[i].status = &status;
		args[i].need = buyers_need + variance - rand()%variance*2;
		pthread_create((pthread_t*)&status.buyers_thread[i], NULL,
			buyer_handler, &args[i]);
	}
	sleep(1);	// Даем время хотя бы одному потоку взять магазин
	int counter = 0;
	int num = rand() % count_shop;
	while(nfree_shops(status.shop_list, count_shop) != count_shop) {
		printf("%s", ch_clear);
		printf(" Магаз.\tТовара\tПокупа.\tПотребн\t№%d\n", counter++);
		//pthread_mutex_lock(&(status.qmutex));
		for (int i = 0; i < count_shop; ++i)
			printf(" [%d]\t%d+%d\t%s(%d)\t%d%s\n", i, 
				status.quantity_of_goods[i], num==i?add:0, 
				status.shop_list[i]?cl_green:cl_normal, 
				status.shop_list[i], status.shop_list[i]?
				args[status.shop_list[i] 
				- 1].need:0, cl_normal);
		printf("\n");
		for (int i = 0; i < count_buyer; ++i) {
			int flag = 1;
			for (int j = 0; j < count_shop; ++j)
				if (status.shop_list[j] == i+1) {
					printf("  %sПокупатель: %d в магазине %d%s\n", 
						cl_green, i + 1, j, cl_normal);
					flag = 0;
					break;
				}
			if (flag)
				printf("  Покупатель: %d спит\n", i + 1);
		}
		status.quantity_of_goods[num] += add;
		num = rand() % count_shop;
		//pthread_mutex_unlock(&(status.qmutex));
		usleep(100000);
	}
	for (int i = 0; i < count_buyer; ++i)
		pthread_join(status.buyers_thread[i], NULL);
	pthread_mutex_destroy(&(status.qmutex));
	return EXIT_SUCCESS;
}
