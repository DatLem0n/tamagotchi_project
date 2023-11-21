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

#ifndef _DISPLAY_DOGM1286_H_
#define _DISPLAY_DOGM1286_H_

#include <ti/drivers/SPI.h>
#include <ti/mw/display/Display.h>
#include <ti/mw/lcd/LCDDogm1286.h>
#include <stdint.h>
#include <xdc/std.h>

extern const Display_FxnTable DisplayDogm1286_fxnTable;

typedef struct DisplayDogm1286_HWAttrs
{
    LCD_Handle lcdHandle;
    PIN_Id     powerPin;
} DisplayDogm1286_HWAttrs;


typedef struct DisplayDogm1286_Object
{
    PIN_State             pinState;
    PIN_Handle            hPins;
    LCD_Handle            hLcd;
    Display_LineClearMode lineClearMode;
    Semaphore_Struct      lcdMutex;
    Char                  lcdBuffer0[1024];
    LCD_Buffer            lcdBuffers[1];
} DisplayDogm1286_Object, *DisplayDogm1286_Handle;


Display_Handle DisplayDogm1286_open(Display_Handle,
                                    Display_Params * params);
void           DisplayDogm1286_clear(Display_Handle handle);
void           DisplayDogm1286_clearLines(Display_Handle handle,
                                          uint8_t fromLine,
                                          uint8_t toLine);
void           DisplayDogm1286_put5(Display_Handle handle, uint8_t line,
                                    uint8_t column, uintptr_t fmt,
                                    uintptr_t a0, uintptr_t a1, uintptr_t a2,
                                    uintptr_t a3, uintptr_t a4);
void DisplayDogm1286_close(Display_Handle);
int DisplayDogm1286_control(Display_Handle handle, unsigned int cmd, void *arg);
unsigned int   DisplayDogm1286_getType(void);
#endif // _DISPLAY_DOGM1286_H_
