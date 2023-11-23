#include <stdio.h>
#include <string.h>
#include "shared.h"
#include <ti/drivers/PIN.h>

// Writing to messageBuffer and sensorDataArray
int write_to_messageBuffer(char* buffer, char* message);
void write_sensor_readings_to_messageBuffer(char* buffer, int time, float ax, float ay, float az, float gx, float gy, float gz, double temp, double press, double light);
void write_sensor_readings_to_sensorDataArray(float sensorDataArray[][SENSOR_DATA_COLUMNS], int index, int time, float ax, float ay, float az, float gx, float gy, float gz, double temp, double press, double light);

// Data cleaning functions
int clean_mpu9250_data(float* ax, float* ay, float* az, float* gx, float* gy, float* gz);

// Tamagotchi functions
int eat(int amount, char* buffer);
int exercise(int amount, char* buffer);
int pet(int amount, char* buffer);
int activate(int eat, int exercise, int pet, char* buffer);

// Sound functions
int noteToFreq(int note);
int makeSound(PIN_Handle buzzerHandle, int soundSelection);
void nowPlaying(enum Music musicSelection, char* buffer);


// Led functions
int blinkLed(PIN_Handle ledHandle, int ledSelection, int blinkTimes, float timesPerSecond);
int turnOnLed(PIN_Handle ledHandle, int ledSelection, float time);
int toggleLed(PIN_Handle ledHandle, char board_led);

/*
* Currently unused functions
*/
void writeOtherSensorsToMsgBuffer(char* buffer, double temp, double press, double light);
void write_mpu9250_to_messageBuffer(char* buffer, int time, float ax, float ay, float az, float gx, float gy, float gz);
void write_mpu9250_to_sensor_data(float sensorDataArray[][SENSOR_DATA_COLUMNS], int* index, float* ax, float* ay, float* az, float* gx, float* gy, float* gz);
