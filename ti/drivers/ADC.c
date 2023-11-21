/*
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== ADC.c ========
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <ti/drivers/ADC.h>

/* Externs */
extern const ADC_Config ADC_config[];

/* Used to check status and initialization */
static int_fast16_t ADC_count = -1;

/* Default ADC parameters structure */
const ADC_Params ADC_defaultParams = {
        .custom = NULL
};

/*
 *  ======== ADC_close ========
 */
void ADC_close(ADC_Handle handle)
{
    handle->fxnTablePtr->closeFxn(handle);
}

/*
 *  ======== ADC_control ========
 */
int_fast16_t ADC_control(ADC_Handle handle, uint_fast16_t cmd, void *arg)
{
    return (handle->fxnTablePtr->controlFxn(handle, cmd, arg));
}

/*
 *  ======== ADC_convert ========
 */
int_fast16_t ADC_convert(ADC_Handle handle, uint16_t *value)
{
    return (handle->fxnTablePtr->convertFxn(handle, value));
}

/*
 *  ======== ADC_convertRawToMicroVolts ========
 */
uint32_t ADC_convertRawToMicroVolts(ADC_Handle handle, uint16_t rawAdcValue)
{
    return (handle->fxnTablePtr->convertRawToMicroVolts(handle, rawAdcValue));
}

/*
 *  ======== ADC_init ========
 */
void ADC_init(void)
{
    if (ADC_count == -1) {
        /* Call each driver's init function */
        for (ADC_count = 0; ADC_config[ADC_count].fxnTablePtr != NULL; ADC_count++) {
            ADC_config[ADC_count].fxnTablePtr->initFxn((ADC_Handle)&(ADC_config[ADC_count]));
        }
    }
}

/*
 *  ======== ADC_open ========
 */
ADC_Handle ADC_open(uint_fast16_t index, ADC_Params *params)
{
    ADC_Handle handle;

    if ((int_fast16_t)index >= ADC_count) {
        return (NULL);
    }

    /* Get handle for this driver instance */
    handle = (ADC_Handle)&(ADC_config[index]);

    return (handle->fxnTablePtr->openFxn(handle, params));
}

/*
 *  ======== ADC_Params_init ========
 */
void ADC_Params_init(ADC_Params *params)
{
    *params = ADC_defaultParams;
}
