#include <stdio.h>
#include <string.h>

void writeMessageBuffer(char* message, char* buffer);
void write_sensor_data_to_messageBuffer(char* buffer, int* time, double* ax, double* ay, double* az, double* gx, double* gy, double* gz, double* temp, double* press, double* light);
void write_other_sensors_to_sensor_data(double** sensor_data, int* index, double* temp, double* press, double* light);
void write_mpu9250_to_sensor_data(double** sensor_data, int* index, double* ax, double* ay, double* az, double* gx, double* gy, double* gz);
int eat(int amount, char* buffer);
int exercise(int amount, char* buffer);
int pet(int amount, char* buffer);
int activate(int eat, int exercise, int pet, char* buffer);

int noteToFreq(int note);
int makeSound(int sound);

