#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ti/sysbios/knl/Clock.h>
#include "sensortag_examples/buzzer.h"
#include "shared.h"
#include "soundbank.h"



#define GROUP_ID_STRING "id:3430"


void writeMessageBuffer(char* message, char* buffer)
{
    if (strlen(buffer) + strlen(message) < BUFFERSIZE) {
        // Jos viestibufferi on tyhjä, lisätään alkuun ryhmän id
        if (buffer[0] == '\0') {
            strcpy(buffer, GROUP_ID_STRING);
            strcat(buffer, ",");
        }
        else {
            strcat(buffer, ",");
        }
        strcat(buffer, message);
    }
}

//ohjeet: https://github.com/UniOulu-Ubicomp-Programming-Courses/jtkj-sensortag-gateway#sending-raw-sensor-data
void write_mpu9250_to_messageBuffer(char* buffer, int* time, float* ax, float* ay, float* az, float* gx, float* gy, float* gz) {
    char msg[BUFFERSIZE];
    sprintf(msg, "time:%i,ax:%.2f,ay:%.2f,az:%.2f,gx:%.2f,gy:%.2f,gz:%.2f",
        *time, *ax, *ay, *az, *gx, *gy, *gz);
    writeMessageBuffer(msg, buffer);
}

/*
 * writes mpu9250 sensor measurements to the sensor_data array
 */
enum SensorDataKeys { TIME, AX, AY, AZ, GX, GY, GZ, TEMP, PRESS, LIGHT };
void write_mpu9250_to_sensor_data(float sensor_data[][SENSOR_DATA_COLUMNS], int* index, float* ax, float* ay, float* az, float* gx, float* gy, float* gz) {
    sensor_data[*index][AX] = *ax;
    sensor_data[*index][AY] = *ay;
    sensor_data[*index][AZ] = *az;
    sensor_data[*index][GX] = *gx;
    sensor_data[*index][GY] = *gy;
    sensor_data[*index][GZ] = *gz;
}
void write_other_sensors_to_sensor_data(float sensor_data[][SENSOR_DATA_COLUMNS], int* index, double* temp, double* press, double* light) {
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
 *Returns frequency of note
 * @param note char (e.g. 'C' for 261 hz)
 * @return frequency value (int)
 */
int noteToFreq(char* note)
{
    int frequency;
    switch (note[0])
    {
    case 'C':
    {
        frequency = 261;
    }break;
    case 'c':
    {
        frequency = 523;
    }break;
    case 'D':
    {
        frequency = 293;
    }break;
    case 'd':
    {
        frequency = 587;
    }break;
    case 'E':
    {
        frequency = 329;
    }break;
    case 'e':
    {
        frequency = 659;
    }break;
    case 'F':
    {
        frequency = 349;
    }break;
    case 'f':
    {
        frequency = 698;
    }break;
    case 'G':
    {
        frequency = 392;
    }break;
    case 'g':
    {
        frequency = 783;
    }break;
    case 'A':
    {
        frequency = 440;
    }break;
    case 'a':
    {
        frequency = 880;
    }break;
    case 'B':
    {
        frequency = 493;
    }break;
    case 'b':
    {
        frequency = 987;
    }break;
    default:
        frequency = 0;
    }
    if (note[1] == '#') {
        frequency = (float)frequency * 1.06;
    }

    return frequency;

}





 /**
  * selected sound WIP
  * @param sound
  * @param songLength
  * @return
  */


// Kutsu makeSoundia project_mainin buzzerTaskissä
  /**
   * Plays the selected sound,
   * @param sound Note array with notes and lengths of notes
   * @param songLength number of notes in the sound array
   * @param tempo playback speed (mess around with values for this to achieve wanted speed)
   * @return
   */
int makeSound(PIN_Handle buzzerHandle, struct Note sound[], int songLength, int tempo) {
    int i;
    for (i = 0; i < songLength; ++i) {
        int frequency = noteToFreq(sound[i].note);
        int duration = 1 / sound[i].length; // in notes

        buzzerOpen(buzzerHandle);
        buzzerSetFrequency(frequency);
        Task_sleep(tempo * duration /  Clock_tickPeriod);
        buzzerClose();
    }
}
