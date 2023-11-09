#include <stdio.h>
#include <string.h>
#include <sensortag_examples/buzzer.h>

char[80] msg;

struct note{
    int pitch;
    int length;
};

void writeBuffer(char* message, char* buffer){
    if (strlen(buffer)!= 0){
        strcat(buffer,",");
    }
    strcat(buffer, message);
}

/**
 * sends UART to raise tamagotchi food level
 * @param amount
 * @return 1 on success, 0 on fail
 */
int eat(int amount, char* buffer){
    if (amount < 1 ) return 0;
    sprintf(msg, "EAT:%i", amount);
    writeBuffer(msg, buffer);
    return 1;
}

/**
 * sends UART to raise tamagotchi exercise level
 * @param amount
 * @return 1 on success, 0 on fail
 */
int exercise(int amount, char* buffer){
    if (amount < 1 ) return 0;
    sprintf(msg, "EXERCISE:%i", amount);
    writeBuffer(msg, buffer);
    return 1;
}

/**
 * sends UART to raise tamagotchi pet level
 * @param amount
 * @return 1 on success, 0 on fail
 */
int pet(int amount, char* buffer){
    if (amount < 1 ) return 0;
    sprintf(msg, "PET:%i", amount);
    writeBuffer(msg, buffer);
    return 1;
}

/**
 * sends UART to raise
 * @param eat amount (int) to raise level
 * @param exercise amount (int) to raise level
 * @param pet amount (int) to raise level
 * @return 1 on success, 0 on fail
 */
int activate(int eat, int exercise, int pet){
    sprintf(msg, "ACTIVATE:%i;%i;%i", eat, exercise, pet);
    writeBuffer(msg, buffer);
    return 0;
}
/**
 *
 * @param note
 * @return
 */
int noteToFreq(int note){
    switch (note) {
        case 0:{
            return 440; //note frequencies here
        }
    }
}
/**
 * selected sound WIP
 * @param sound
 * @return
 */
int makeSound(int sound){
    return;
}