#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
void write_sensor_data_to_messageBuffer(char* buffer, int* time, double* ax, double* ay, double* az, double* gx, double* gy, double* gz, double* temp, double* press, double* light) {
    char msg[30];
    sprintf(msg, "time:%i,ax:%.2f,ay:%.2f,az:%.2f,gx:%.2f,gy:%.2f,gz:%.2f,temp:%.2f,press:%.2f,light:%.2f",
        ax, ay, az, gx, gy, gz, temp, press, light);
    writeMessageBuffer(msg, buffer);
}

/*
 * writes mpu9250 sensor measurements to the sensor_data array
 */
enum SensorDataKeys { TIME, AX, AY, AZ, GX, GY, GZ, TEMP, PRESS, LIGHT };
void write_mpu9250_to_sensor_data(double** sensor_data, int* index, double* ax, double* ay, double* az, double* gx, double* gy, double* gz) {
    sensor_data[*index][AX] = *ax;
    sensor_data[*index][AY] = *ay;
    sensor_data[*index][AZ] = *az;
    sensor_data[*index][GX] = *gx;
    sensor_data[*index][GY] = *gy;
    sensor_data[*index][GZ] = *gz;
}
void write_other_sensors_to_sensor_data(double** sensor_data, int* index, double* temp, double* press, double* light) {
    sensor_data[*index][TEMP] = *temp;
    sensor_data[*index][PRESS] = *press;
    sensor_data[*index][LIGHT] = *light;
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
struct Note
{
    char note[3];
    int length;
};
/**
 *Returns frequency of note, # is marked with lowercase letters (horrible, I know...)
 * @param note char (e.g. 'C' for 261 hz)
 * @return frequency value (int)
 */
int noteToFreq(char * note)
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
    if (note[1] == '#'){
        frequency = (float)frequency * 1.06;
    }

    return frequency;

}

/**
 *          SOUNDBANK
 */
int SOUND_AMOUNT = 1;
struct Note Doom[] ={
        {"E",6},
        {"E",8},
        {"e",8},
        {"E",6},
        {"E",8},
        {"d",8},
        {"E",6},
        {"E",8},
        {"c",8},
        {"E",6},
        {"E",8},
        {"A#",8},
        {"E",6},
        {"E",8},
        {"B",8},
        {"c",8},
        {"E",6},
        {"E",8},
        {"e",8},
        {"E",6},
        {"E",8},
        {"d",8},
        {"E",6},
        {"E",8},
        {"c",8},
        {"E",6},
        {"E",8},
        {"A#",4},
        {"-",1}
};

/**
 *          END OF SOUNDBANK (for now)
 */




/**
 * selected sound WIP
 * @param sound
 * @param songLength
 * @return
 */
int makeSound(struct Note sound[], int songlength){
    for (int i = 0; i < songlength; ++i) {
        int frequency = noteToFreq(sound[i].note);
        int duration = sound[i].length;

    }
}