#ifndef _SOUNDBANK_H_
#define _SOUNDBANK_H_
struct Note
{
    char note[3];
    int length;
};
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
        {"-",1}
} doom;

/**
 *          END OF SOUNDBANK (for now)
 */
#endif