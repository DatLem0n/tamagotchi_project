#ifndef SHARED_H
#define SHARED_H

#include <ti/sysbios/knl/Clock.h>

#define ON 1
#define OFF 0
#define STACKSIZE 2048
#define BUFFERSIZE 80

#define SECOND (1000000.0 / Clock_tickPeriod)
// TODO: Suurenna jos muistia riittää
#define SENSOR_DATA_ROWS 10
#define SENSOR_DATA_COLUMNS 10

#define GROUP_ID_STRING "id:3430"
#define GROUP_ID_NUM "3430"

enum SensorDataKeys { TIME, AX, AY, AZ, GX, GY, GZ, TEMP, PRESS, LIGHT };
enum Music { SILENT, DOOM, VICTORY, ROUNDABOUT, EAT, END };
int Beeping = 0;

#endif
