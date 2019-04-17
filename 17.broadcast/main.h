#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>		//fprintf
#include <stdlib.h>		//EXIT_SUCCESS
#include <stdint.h>
//#include <.h>

#include "net.h"
#include "oth.h"

#define BCPORT 32000	// Порт для анонсов
#define PORTA 33000		// Порт для обмена между серверами
#define NUMPORTS MAXSERVERS - 1		// Число портов для серверов

#define MAXPENDING 10	// Очередь на подключение

#endif
