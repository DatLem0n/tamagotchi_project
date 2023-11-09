#include <stdio.h>
#include <string.h>
#include <sensortag_examples/buzzer.h>

#define GROUP_ID_STRING = "id:30";

char[80] msg;

void writeMessageBuffer(char *message, char *buffer)
{
    // Jos viestibufferi on tyhjä, lisätään alkuun ryhmän id
    if (strlen(buffer) == 0)
        strcpy(buffer, GROUP_ID_STRING);

    if (strlen(buffer) != 0)
    {
        strcat(buffer, ",");
    }
    strcat(buffer, message);
}

/**
 * sends UART to raise tamagotchi food level
 * @param amount
 * @return 1 on success, 0 on fail
 */
int eat(int amount, char *buffer)
{
    if (amount < 1)
        return 0;
    sprintf(msg, "EAT:%i", amount);
    writeMessageBuffer(msg, buffer);
    return 1;
}

/**
 * sends UART to raise tamagotchi exercise level
 * @param amount
 * @return 1 on success, 0 on fail
 */
int exercise(int amount, char *buffer)
{
    if (amount < 1)
        return 0;
    sprintf(msg, "EXERCISE:%i", amount);
    writeMessageBuffer(msg, buffer);
    return 1;
}

/**
 * sends UART to raise tamagotchi pet level
 * @param amount
 * @return 1 on success, 0 on fail
 */
int pet(int amount, char *buffer)
{
    if (amount < 1)
        return 0;
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
int activate(int eat, int exercise, int pet)
{
    sprintf(msg, "ACTIVATE:%i;%i;%i", eat, exercise, pet);
    writeMessageBuffer(msg, buffer);
    return 0;
}