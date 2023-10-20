#include <stdio.h>
// #include <tehtävässä halutut>
#include <inttypes.h>
#include <math.h>

// Prototyyppi tähän
// float lampotila(uint16_t rekisteri, float kerroin);
// float kosteus(uint16_t rekisteri);



float valoisuus(uint16_t rekisteri) {
    uint16_t MASK_E = 0b1111000000000000;
    uint16_t MASK_R = ~MASK_E;

    uint16_t E = rekisteri & MASK_E;
    E = E >> 12;

    uint16_t R = rekisteri & MASK_R;

    return 0.01 * pow(2,E) * R;
}
