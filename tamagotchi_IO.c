#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/System.h>
#include "sensortag_examples/buzzer.h"
#include "shared.h"

#define GROUP_ID_STRING "id:3430"


struct Note
{
    char note[3];
    int length;
};

/**
 * Writes given message to the messagebuffer. Will try writing untill buffer is not full.
 * @param message
 * @param buffer
 * @return 1 on success
 */
int writeMessageBuffer(char* buffer, char* message)
{
    while (1){
        if (strlen(buffer) + strlen(message) < BUFFERSIZE) {
            // Jos viestibufferi on tyhjä, lisätään alkuun ryhmän id
            if (buffer[0] == '\0') {
                strcpy(buffer, GROUP_ID_STRING);
            }
            strcat(buffer, ",");
            strcat(buffer, message);
            return 1;
        }
        Task_sleep(SECOND / 10);
    }
}

//ohjeet: https://github.com/UniOulu-Ubicomp-Programming-Courses/jtkj-sensortag-gateway#sending-raw-sensor-data
 /**
  * Writes all sensor data to the buffer
  * @param buffer
  * @param time time of measurement
  * @param ax accelerometer x
  * @param ay accelerometer y
  * @param az accelerometer z
  * @param gx gyro x
  * @param gy gyro y
  * @param gz gyro z
  * @param temp temperature
  * @param press pressure
  * @param light light
  */
void writeSensorsToMsgBuffer(char* buffer, int time, float ax, float ay, float az, float gx, float gy, float gz, double temp, double press, double light) {
    int dataAmount = 10;
    int bufferFull;
    char msg[BUFFERSIZE];
    char dataPrefixes[10][20] = {"ax:", "ay:", "az:", "gx:", "gy:", "gz:", "temp:", "press:", "light:"};
    double dataPointerArray[10] = {(double) ax, (double) ay, (double) az, (double) gx, (double) gy,
                                     (double) gz, temp, press, light};

    int i = 0;
     for (; i < dataAmount; ++i) {
         if (i == 0) {
             snprintf(msg, BUFFERSIZE, "%s%i", "time:", time);
         }
         else{
             snprintf(msg,BUFFERSIZE, "%s%.02f", dataPrefixes[i-1], dataPointerArray[i-1]);
         }
         do{
             bufferFull = !writeMessageBuffer(buffer, msg);
         } while (bufferFull);
     }
}

void write_mpu9250_to_messageBuffer(char* buffer, int time, float ax, float ay, float az, float gx, float gy, float gz) {
    char msg[BUFFERSIZE];
    sprintf(msg, "time:%i,ax:%.2f,ay:%.2f,az:%.2f,gx:%.2f,gy:%.2f,gz:%.2f",
        time, ax, ay, az, gx, gy, gz);
    writeMessageBuffer(buffer, msg);
}
void writeOtherSensorsToMsgBuffer(char *buffer, double temp, double press, double light){
    char msg[BUFFERSIZE];
    sprintf(msg,"temp:%f,press:%f,light:%f", temp, press, light);
    writeMessageBuffer(buffer, msg);
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
    writeMessageBuffer(buffer, msg);
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
    writeMessageBuffer(buffer, msg);
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
    writeMessageBuffer(buffer, msg);
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
    writeMessageBuffer(buffer, msg);
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
        frequency = 261;
        break;
    case 'c':
        frequency = 523;
        break;
    case 'D':
        frequency = 293;
        break;
    case 'd':
        frequency = 587;
        break;
    case 'E':
        frequency = 329;
        break;
    case 'e':
        frequency = 659;
        break;
    case 'F':
        frequency = 349;
        break;
    case 'f':
        frequency = 698;
        break;
    case 'G':
        frequency = 392;
        break;
    case 'g':
        frequency = 783;
        break;
    case 'A':
        frequency = 440;
        break;
    case 'a':
        frequency = 880;
        break;
    case 'B':
        frequency = 493;
        break;
    case 'b':
        frequency = 987;
        break;
    default:
        frequency = 0;
    }

    if (note[1] == '#') {
        frequency = (float)frequency * 1.06;
    }

    return frequency;

}

/**
 *          SOUNDBANK
 */
struct Note Doom[] = {
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
        {"-",4}
};

struct Note Victory[] = {
        {"c",8},
        {"c",8},
        {"c",8},
        {"c",4},
        {"G#",4},
        {"A#",4},
        {"c",6},
        {"A#",16},
        {"c",2},
        {"-",4}
};
struct Note toBeContinued[] = {
        {"f#", 4},
        {"g", 8},
        {"g", 16},
        {"f#", 16},
        {"e", 16},
        {"d", 8},
        {"e", 8},
        {"d", 8},
        {"c", 8},
        {"H", 8},
        {"d", 8},
        {"H", 8},
        {"c", 8},
        {"A", 8},
        {"H", 8},
        {"G", 8},
        {"A", 8},
        {"e", 4},
        {"e", 8},
        {"f#",4},
        {"g", 4},
        {"f#", 4},
        {"g", 4},
        {"f#", 8},
        {"e", 2},
        {"e", 4},
        {"e", 8},
        {"f#", 4},
        {"g", 4},
        {"f#", 8},
        {"e", 2},
        {"-", 3},
        {"e", 4},
        {"e", 8},
        {"f#",4},
        {"g", 4},
        {"f#", 4},
        {"g", 4},
        {"f#", 8},
        {"e", 2},
        {"e", 4},
        {"e", 8},
        {"f#", 4},
        {"g", 4},
        {"f#", 8},
        {"e", 2},
        {"-", 3}
};

/**
 *          END OF SOUNDBANK (for now)
 */

  /**
   * Plays the selected sound,
   * Sound selection:
   *    0 for silence
   *    1 for Doom
   *    2 for Victory sound
   *    3 for roundabout
   *
   * @param buzzerHandle
   * @param soundSelection (int)
   *
   * @return
   */
int makeSound(PIN_Handle buzzerHandle, int soundSelection) {
    struct Note* sound;
    int songLength;
    int tempo;
      switch (soundSelection) {
          case 1:
              sound = Doom;
              songLength = sizeof(Doom)/sizeof(struct Note);
              tempo = SECOND;
              break;

          case 2:
              sound = Victory;
              songLength = sizeof(Victory)/sizeof(struct Note);
              tempo = SECOND * 1.5;
              break;
          case 3:
              sound = toBeContinued;
              songLength = sizeof(toBeContinued)/sizeof(struct Note);
              tempo = 2*SECOND;
              break;
          default:
              return 0;
      }

    int i;
    for (i = 0; i < songLength; ++i) {
        if(soundSelection == SILENT)
            break;
        buzzerOpen(buzzerHandle);
        int frequency = noteToFreq(sound[i].note);
        float duration = 1.0 / sound[i].length; // in notes

        buzzerSetFrequency(frequency);
        Task_sleep((float) tempo * duration);
        buzzerClose();
        Task_sleep(tempo * 0.0001);

    }

    return 1;
}

int blinkLed(PIN_Handle ledHandle, int ledSelection, int blinkTimes, float timesPerSecond) {
    char led;
    switch (ledSelection) {
        case 0:
            led = Board_LED0;
            break;
        case 1:
            led = Board_LED1;
            break;
        default:
            return 0;
    }
    int i;
    for (i = 0; i < blinkTimes; ++i) {
        int pinValue = PIN_getOutputValue(led);
        pinValue = !pinValue;
        PIN_setOutputValue(ledHandle, led, pinValue);
        Task_sleep(SECOND / timesPerSecond);
    }
    PIN_setOutputValue(ledHandle, led, 0);

    return 1;
}

int turnOnLed(PIN_Handle ledHandle, int ledSelection, float time) {
    char led;
    switch (ledSelection) {
        case 0:
            led = Board_LED0;
            break;
        case 1:
            led = Board_LED1;
            break;
        default:
            return 0;
    }
    PIN_setOutputValue(ledHandle, led, 1);
    Task_sleep(SECOND * time);
    PIN_setOutputValue(ledHandle, led, 0);

    return 1;
}

int toggleLed(PIN_Handle ledHandle, int ledSelection) {
    char led;
    switch (ledSelection) {
        case 0:
            led = Board_LED0;
            break;
        case 1:
            led = Board_LED1;
            break;
        default:
            return 0;
    }
    bool isOn = PIN_getOutputValue(led);
    isOn = !isOn;
    PIN_setOutputValue(ledHandle, led, isOn);

    return 1;
}