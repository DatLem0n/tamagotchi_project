#include <stdio.h>
#include <string.h>

void writeMessageBuffer(char *message, char *buffer);
int eat(int amount, char *buffer);
int exercise(int amount, char *buffer);
int pet(int amount, char *buffer);
int activate(int eat, int exercise, int pet, char *buffer);

int noteToFreq(int note);
int makeSound(int sound);

