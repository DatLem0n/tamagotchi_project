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
void write_mpu9250_to_messageBuffer(char* buffer, int* time, float* ax, float* ay, float* az, float* gx, float* gy, float* gz) {
    char msg[80];
    sprintf(msg, "time:%i,ax:%.2f,ay:%.2f,az:%.2f,gx:%.2f,gy:%.2f,gz:%.2f",
        ax, ay, az, gx, gy, gz);
    writeMessageBuffer(msg, buffer);
}

/*
 * writes mpu9250 sensor measurements to the sensor_data array
 */
enum SensorDataKeys { TIME, AX, AY, AZ, GX, GY, GZ, TEMP, PRESS, LIGHT };
void write_mpu9250_to_sensor_data(float* sensor_data, int* index, float* ax, float* ay, float* az, float* gx, float* gy, float* gz) {
    sensor_data[*index][AX] = *ax;
    sensor_data[*index][AY] = *ay;
    sensor_data[*index][AZ] = *az;
    sensor_data[*index][GX] = *gx;
    sensor_data[*index][GY] = *gy;
    sensor_data[*index][GZ] = *gz;
}
void write_other_sensors_to_sensor_data(float* sensor_data, int* index, double* temp, double* press, double* light) {
    sensor_data[*index][TEMP] = (float)*temp;
    sensor_data[*index][PRESS] = (float)*press;
    sensor_data[*index][LIGHT] = (float)*light;
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

/**
 * note is the char value of frequency (C,D etc.)
 * length of note is 1/x so 1/4th note would be length of 4
 */
struct note
{
    char note;
    int length;
};
/**
 *Returns frequency of note, # is marked with lowercase letters (horrible, I know...)
 * @param note char (e.g. 'C' for 261 hz)
 * @return frequency value (int)
 */
int noteToFreq(char note)
{
    switch (note)
    {
    case 'C':
    {
        return 261;
    }break;
    case 'c':
    {
        return 277;
    }break;
    case 'D':
    {
        return 293;
    }break;
    case 'd':
    {
        return 311;
    }break;
    case 'E':
    {
        return 329;
    }break;
    case 'F':
    {
        return 349;
    }break;
    case 'f':
    {
        return 362;
    }break;
    case 'G':
    {
        return 392;
    }break;
    case 'g':
    {
        return 415;
    }break;
    case 'A':
    {
        return 440;
    }break;
    case 'a':
    {
        return 466;
    }break;
    case 'H':
    {
        return 493;
    }break;
        default:
            return 0;
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