#include <stdio.h>
#include <string.h>
#include "sensortag_examples/buzzer.h"

#define GROUP_ID_STRING "id:30"

void writeMessageBuffer(char* message, char* buffer)
{
    if (strlen(buffer) + strlen(message) < 80) {
        // Jos viestibufferi on tyhjä, lisätään alkuun ryhmän id
        if (strlen(buffer) == 0) {
            strcpy(buffer, GROUP_ID_STRING);
        }
        else {
            strcat(buffer, ",");
        }
        strcat(buffer, message);
    }
}

//ohjeet: https://github.com/UniOulu-Ubicomp-Programming-Courses/jtkj-sensortag-gateway#sending-raw-sensor-data
void write_sensor_data_to_messageBuffer(char* buffer, int* time, float* ax, float* ay, float* az, float* gx, float* gy, float* gz, float* temp, float* press, float* light){
char msg[30];
sprintf(msg, "time:%i,ax:%.2f,ay:%.2f,az:%.2f,gx:%.2f,gy:%.2f,gz:%.2f,temp:%.2f,press:%.2f,light:%.2f",
    ax, ay, az, gx, gy, gz, temp, press, light);
writeMessageBuffer(msg, buffer);
}

/*
 * writes mpu9250 sensor measurements to the sensor_data array
 */
enum SensorDataKeys { TIME, AX, AY, AZ, GX, GY, GZ, TEMP, PRESS, LIGHT };
void write_mpu9250_to_sensor_data(float* sensor_data, int index, float* ax, float* ay, float* az, float* gx, float* gy, float* gz) {
    sensor_data[index][AX] = ax;
    sensor_data[index][AY] = ay;
    sensor_data[index][AZ] = az;
    sensor_data[index][GX] = gx;
    sensor_data[index][GY] = gy;
    sensor_data[index][GZ] = gz;
}
void write_other_sensors_to_sensor_data(float* sensor_data, int index, float* temp, float* press, float* light){
    sensor_data[index][TEMP] = temp;
    sensor_data[index][PRESS] = press;
    sensor_data[index][LIGHT] = light;
}

/**
 * sends UART to raise tamagotchi food level
 * @param amount
 * @return 1 on success, 0 on fail
 */
int eat(int amount, char* buffer)
{
    if (amount < 1)
        return 0;

    char msg[10];
    sprintf(msg, "EAT:%i", amount);
    writeMessageBuffer(msg, buffer);
    return 1;
}

/**
 * sends UART to raise tamagotchi exercise level
 * @param amount
 * @return 1 on success, 0 on fail
 */
int exercise(int amount, char* buffer)
{
    if (amount < 1)
        return 0;

    char msg[10];
    sprintf(msg, "EXERCISE:%i", amount);
    writeMessageBuffer(msg, buffer);
    return 1;
}

/**
 * sends UART to raise tamagotchi pet level
 * @param amount
 * @return 1 on success, 0 on fail
 */
int pet(int amount, char* buffer)
{
    if (amount < 1)
        return 0;

    char msg[10];
    sprintf(msg, "PET:%i", amount);
    writeMessageBuffer(msg, buffer);
    return 1;
}

/**
 * sends UART to raise
 * @param eat amount (int) to raise level
 * @param exercise amount (int) to raise level
 * @param pet amount (int) to raise level
 * @return 1 on success, 0 on fail
 */
int activate(int eat, int exercise, int pet, char* buffer)
{
    if (eat < 1 || exercise < 1 || pet < 1 || buffer == NULL)
        return 0;

    char msg[30];
    sprintf(msg, "ACTIVATE:%i;%i;%i", eat, exercise, pet);
    writeMessageBuffer(msg, buffer);
    return 0;
}

struct note
{
    int pitch;
    int length;
};
/**
 *
 * @param note
 * @return
 */
int noteToFreq(int note)
{
    switch (note)
    {
    case 0:
    {
        return 440; // note frequencies here
    }
    }
}
/**
 * selected sound WIP
 * @param sound
 * @return
 */
int makeSound(int sound)
{
    return;
}