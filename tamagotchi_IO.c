#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/System.h>
#include "sensortag_examples/buzzer.h"
#include "shared.h"
#include "ti/drivers/UART.h"

/**
 * Writes given message to the message buffer. Will try writing until buffer is not full.
 * @param message
 * @param buffer
 * @return 1 on success
 */
int write_to_messageBuffer(char* buffer, char* message)
{
    while (1) {
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
void write_sensor_readings_to_messageBuffer(char* buffer, int time, float ax, float ay, float az, float gx, float gy, float gz, double temp, double press, double light) {
    int dataAmount = 10;
    int bufferFull;
    char msg[BUFFERSIZE];
    char dataPrefixes[10][20] = { "ax:", "ay:", "az:", "gx:", "gy:", "gz:", "temp:", "press:", "light:" };
    double dataPointerArray[10] = { (double)ax, (double)ay, (double)az, (double)gx, (double)gy,
                                     (double)gz, temp, press, light };

    int i = 0;
    for (; i < dataAmount; ++i) {
        if (i == 0) {
            snprintf(msg, BUFFERSIZE, "%s%i", "time:", time);
        }
        else {
            snprintf(msg, BUFFERSIZE, "%s%.02f", dataPrefixes[i - 1], dataPointerArray[i - 1]);
        }
        do {
            bufferFull = !write_to_messageBuffer(buffer, msg);
        } while (bufferFull);
    }
}

// If a mpu9250 measurement is outside the threshold, forces it back inside range [-threshold,threshold] 
int clean_mpu9250_data(float* ax, float* ay, float* az, float* gx, float* gy, float* gz) {
    float threshold = 5;
    uint8_t data_amount = 6;
    float* data[6] = { ax, ay, az, gx, gy, gz };

    uint8_t i;
    for (i = 0; i < data_amount; i++) {
        if (*data[i] < -threshold)
            *data[i] = -threshold;
        if (*data[i] > threshold)
            *data[i] = threshold;
    }
    return 1;
}

void calculate_mpu9250_deltas(float sensorDataArray[][SENSOR_DATA_COLUMNS], float mpu9250DeltasArray[6]){
    uint8_t t1, t2;
    // Make sure that t1 < t2. Because the sensorDataArray is a ring buffer, the measurements might not be in ascending time order. 
    if(sensorDataArray[0][TIME] < sensorDataArray[1][TIME]){
        t1 = 0;
        t2 = 1;
    }
    else{
        t1 = 1;
        t2 = 0;
    }

    // Calculate the change in mpu9250 measurements between timesteps
    uint8_t i = AX;
    for(i = AX; i <= GZ; i++){
        mpu9250DeltasArray[i] = sensorDataArray[t1][i] - sensorDataArray[t2][i];
    }

}

uint8_t axBounces=0, ayBounces=0, azBounces=0;
bool detect_Exercise(float mpu9250DeltasArray[6]){
    float d_ax = mpu9250DeltasArray[AX];
    float d_ay = mpu9250DeltasArray[AY];
    float d_az = mpu9250DeltasArray[AZ];

    if(d_ax < -0,2 || d_ax > 0,2)
        axBounces++;
    if(d_ay < -0,2 || d_ay > 0,2)
        ayBounces++;
    if(d_az < -0,2 || d_az > 0,2)
        azBounces++;
    
    if(axBounces > 10){
        return true;
        axBounces = 0;
    }
    if(ayBounces > 10){
        return true;
        ayBounces = 0;}
    if(azBounces > 10){
        return true
        azBounces = 0;}
    
    return false;
}


void write_sensor_readings_to_sensorDataArray(float sensorDataArray[][SENSOR_DATA_COLUMNS], int index, int time, float ax, float ay, float az,
    float gx, float gy, float gz, double temp, double press, double light) {
    sensorDataArray[index][TIME] = (float)time;
    sensorDataArray[index][AX] = ax;
    sensorDataArray[index][AY] = ay;
    sensorDataArray[index][AZ] = az;
    sensorDataArray[index][GX] = gx;
    sensorDataArray[index][GY] = gy;
    sensorDataArray[index][GZ] = gz;
    sensorDataArray[index][TEMP] = (float)temp;
    sensorDataArray[index][PRESS] = (float)press;
    sensorDataArray[index][LIGHT] = (float)light;
}

/**
 * sends UART to raise tamagotchi food level
 * @param amount
 * @param buffer
 * @return 1 on success, 0 on fail
 */
int eat(int amount, char* buffer)
{
    if (amount < 1)
        return 0;

    char msg[10];
    sprintf(msg, "EAT:%i", amount);
    write_to_messageBuffer(buffer, msg);
    return 1;
}

/**
 * sends UART to raise tamagotchi exercise level
 * @param amount
 * @param buffer
 * @return 1 on success, 0 on fail
 */
int exercise(int amount, char* buffer)
{
    if (amount < 1)
        return 0;
    char msg[20];
    sprintf(msg, "EXERCISE:%i", amount);
    write_to_messageBuffer(buffer, msg);
    return 1;
}

/**
 * sends UART to raise tamagotchi pet level
 * @param amount
 * @param buffer
 * @return 1 on success, 0 on fail
 */
int pet(int amount, char* buffer)
{
    if (amount < 1)
        return 0;


    char msg[10];
    sprintf(msg, "PET:%i", amount);
    write_to_messageBuffer(buffer, msg);
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
    write_to_messageBuffer(buffer, msg);
    return 1;
}


// TODO: siirrä omaan tiedostoon?
/*
* Music data types and functions
*/

struct Note
{
    char note[3];
    int length;
};

void testMessage(char* buffer){
    char message[20] = "MSG1:Yaas queen";
    write_to_messageBuffer(buffer, message);
    Task_sleep(SECOND);
}

void nowPlaying(int musicSelection, char* buffer){
    char message[80] = "MSG1:";
    switch (musicSelection) {
        case DOOM:
            strcat(message, "Now Playing: Doom");
            break;
        case VICTORY:
            strcat(message, "Now Playing: Victory");
            break;
        case ROUNDABOUT:
            strcat(message, "Now Playing: Roundabout");
            break;
        default:
            strcat(message, "Currently not playing anything.");
            break;
    }
    write_to_messageBuffer(buffer, message);
}

/**
 *Returns frequency of note
 * @param note char (e.g. 'C' for 261 hz)
 * @return frequency value (int)
 */
int noteToFreq(const char* note)
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
    else if (note[1] == 'b'){
        frequency = (float)frequency * 0.945;
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
        {"e", 2}
};

struct Note eatSound[] = {
        {"C", 16},
        {"E", 16},
        {"G", 16},
        {"c", 16}
};

struct Note alert[] = {
        {"Hb", 8},
        {"db", 8},
        {"G", 1}
};

struct Note oneUp[] = {
        {"E",8},
        {"G",8},
        {"e",8},
        {"c",8},
        {"d",8},
        {"g",8}
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
  *    4 is reserved for END in soundSelection (for jukebox)
  *    5 for eatSound
  *    6 for alert sound
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
        songLength = sizeof(Doom) / sizeof(struct Note);
        tempo = SECOND;
        break;
    case 2:
        sound = Victory;
        songLength = sizeof(Victory) / sizeof(struct Note);
        tempo = SECOND * 1.5;
        break;
    case 3:
        sound = toBeContinued;
        songLength = sizeof(toBeContinued) / sizeof(struct Note);
        tempo = 2 * SECOND;
        break;
    case 5:
        sound = eatSound;
        songLength = sizeof(eatSound) / sizeof(struct Note);
        tempo = SECOND;
        break;
    case 6:
        sound = alert;
        songLength = sizeof(alert) / sizeof(struct Note);
        tempo = SECOND;
        break;
    case 7:
        sound = oneUp;
        songLength = sizeof(oneUp) / sizeof(struct Note);
        tempo = SECOND;
        break;
    default:
        Task_sleep(SECOND / 2);
        return 0;
    }

    int i;
    for (i = 0; i < songLength; ++i) {
        buzzerOpen(buzzerHandle);
        int frequency = noteToFreq(sound[i].note);
        float duration = 1.0 / sound[i].length; // in notes

        buzzerSetFrequency(frequency);
        Task_sleep((float)tempo * duration);
        buzzerClose();
        Task_sleep(tempo * 0.0001);

    }

    return 1;
}

/*
* Led functions
*/

// TODO: Siirrä sleepiä vaativat led-funktiot sopivaan taskiin että Task_sleep toimii
// TAI: alusta kaikki ledit ja vaihtele niiden välillä niin että ne näyttävät vilkkuvan samaan aikaan

/**
 * blinks the led for given time and frequency
 * @param ledHandle
 * @param ledSelection
 * @param blinkTimes
 * @param timesPerSecond
 * @return
 */
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

/**
 * turns on the led for given time, then turns it off
 * @param ledHandle
 * @param ledSelection
 * @param time
 * @return
 */
int turnOnLed(PIN_Handle ledHandle, int ledSelection, float time){
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


/**
 * @param ledHandle handle of the led being toggled
 * @param board_led char of the BOARD_LED being toggled
 */
void toggleLed(PIN_Handle ledHandle, char board_led) {

    if (PIN_getOutputValue(board_led) == FALSE) {
        PIN_setOutputValue(ledHandle, board_led, ON);
    }
    else {
        PIN_setOutputValue(ledHandle, board_led, OFF);
    }
}

/*
* Currently unused functions
*/
void write_mpu9250_to_messageBuffer(char* buffer, int time, float ax, float ay, float az, float gx, float gy, float gz) {
    char msg[BUFFERSIZE];
    sprintf(msg, "time:%i,ax:%.2f,ay:%.2f,az:%.2f,gx:%.2f,gy:%.2f,gz:%.2f",
        time, ax, ay, az, gx, gy, gz);
    write_to_messageBuffer(buffer, msg);
}
void writeOtherSensorsToMsgBuffer(char* buffer, double temp, double press, double light) {
    char msg[BUFFERSIZE];
    sprintf(msg, "temp:%f,press:%f,light:%f", temp, press, light);
    write_to_messageBuffer(buffer, msg);
}