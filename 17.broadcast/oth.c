#include "oth.h"

void DieWithError(char *errorMessage) {
	if (errorMessage)
		fprintf(stderr, "%s\n", errorMessage);
	getchar();
	exit(EXIT_FAILURE);
}

void insertion_sort(uint32_t* arr, const uint32_t size) {
	for(uint32_t i = 1, j; i < size; ++i) {
		const uint32_t temp = arr[i];
		for (j = i; j > 0 && temp < arr[j - 1]; --j)
			arr[j] = arr[j - 1];
		arr[j] = temp;
	}
}
