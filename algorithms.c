#include <stdio.h>
#include <inttypes.h>
#include "shared.h"

void moving_average(float *input_array, uint8_t array_size, uint8_t window_size, float *output_array);

/*
int main(void)
{
    //Moving average test
    float testArray[] = {11.67, 12.91, 15.81, 17.21, 13.39, 14.34, 12.72, 17.47, 16.66, 16.05, 14.42, 18.41, 19.02, 18.82, 18.43, 11.61, 17.5, 16.66, 14.02, 15.33, 16.88, 12.58, 12.8, 13.64, 18.99, 19.44};
    float avgArray[sizeof(testArray) / (sizeof(float))];
    moving_average(testArray, 26, 3, avgArray);

    return 0;
}
*/

void moving_average(float *input_array, uint8_t array_size, uint8_t window_size, float *output_array)
{
    if (input_array == NULL || output_array == NULL)
        return;
    float sum = 0;
    uint8_t i;
    for ( i = 0; i <= array_size; i++)
    {
        sum += input_array[i];

        if (i >= window_size)
        {
            sum -= input_array[i - window_size];
        }
        output_array[i] = sum / window_size;
    }
}
