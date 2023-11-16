#include <stdio.h>
#include <string.h>
#include "shared.h"


void writeMessageBuffer(char* message, char* buffer);
void write_mpu9250_to_messageBuffer(char* buffer, int* time, float* ax, float* ay, float* az, float* gx, float* gy, float* gz);
void write_other_sensors_to_sensor_data(float sensor_data[][SENSOR_DATA_COLUMNS], int* index, double* temp, double* press, double* light);
void write_mpu9250_to_sensor_data(float sensor_data[][SENSOR_DATA_COLUMNS], int* index, float* ax, float* ay, float* az, float* gx, float* gy, float* gz);
int eat(int amount, char* buffer);
int exercise(int amount, char* buffer);
int pet(int amount, char* buffer);
int activate(int eat, int exercise, int pet, char* buffer);

int noteToFreq(int note);
int makeSound(PIN_Handle buzzerHandle, struct Note sound[], int songLength, int tempo);

