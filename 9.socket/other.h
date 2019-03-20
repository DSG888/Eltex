#ifndef OTHER_H
#define OTHER_H

//#include "main.h"
#include <stdlib.h>		//rand
#include <sys/time.h>	//gettimeofday
#include <stdio.h>		//fprintf

int getrand(int min, int max);
double wtime();
void DieWithError(char *errorMessage);

#endif
