#ifndef SHARED_H
#define SHARED_H

#include <ti/sysbios/knl/Clock.h>

#define STACKSIZE 2048
#define BUFFERSIZE 80
#define SECOND 1000000 / Clock_tickPeriod
// TODO: Suurenna jos muistia riittää
#define SENSOR_DATA_ROWS 10
#define SENSOR_DATA_COLUMNS 10

enum Music {SILENT, DOOM, VICTORY, ROUNDABOUT, END};

#endif
