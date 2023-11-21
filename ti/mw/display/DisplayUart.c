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

/* -----------------------------------------------------------------------------
 *  Includes
 * -----------------------------------------------------------------------------
 */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/drivers/UART.h>

#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/System.h>

#include <ti/mw/display/Display.h>
#include <ti/mw/display/DisplayUart.h>

#include <string.h>

/* -----------------------------------------------------------------------------
 *  Constants and macros
 * -----------------------------------------------------------------------------
 */
#ifndef MIN
#  define MIN(n, m)    (((n) > (m)) ? (m) : (n))
#endif

/* -----------------------------------------------------------------------------
 *   Type definitions
 * -----------------------------------------------------------------------------
 */


/* -----------------------------------------------------------------------------
 *                           Local variables
 * -----------------------------------------------------------------------------
 */
/* Display function table for UART implementation */
const Display_FxnTable DisplayUart_fxnTable = {
    DisplayUart_open,
    DisplayUart_clear,
    DisplayUart_clearLines,
    DisplayUart_put5,
    DisplayUart_close,
    DisplayUart_control,
    DisplayUart_getType,
};

/* -----------------------------------------------------------------------------
 *                                          Functions
 * -----------------------------------------------------------------------------
 */
/*!
 * @fn          DisplayUart_open
 *
 * @brief       Initialize the UART transport
 *
 * @descr       Opens the UART index specified in the HWAttrs, and creates a
 *              mutex semaphore
 *
 * @param       hDisplay - pointer to Display_Config struct
 * @param       params - display parameters
 *
 * @return      Pointer to Display_Config struct
 */
Display_Handle DisplayUart_open(Display_Handle hDisplay,
                                Display_Params *params)
{
    DisplayUart_HWAttrs *hwAttrs = (DisplayUart_HWAttrs *)hDisplay->hwAttrs;
    DisplayUart_Object  *object  = (DisplayUart_Object  *)hDisplay->object;

    UART_Params uartParams;
    UART_Params_init(&uartParams);
    uartParams.baudRate  = hwAttrs->baudRate;
    uartParams.writeMode = UART_MODE_BLOCKING;

    Semaphore_Params semParams;
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    Semaphore_construct(&object->mutex, 1, &semParams);

    object->hUart = UART_open(hwAttrs->uartIdx, &uartParams);
    if (NULL == object->hUart)
    {
        Log_print0(Diags_USER1, "DisplayUart.c: Couldn't open UART");
        return NULL;
    }

    return hDisplay;
}


/*!
 * @fn          DisplayUart_clear
 *
 * @brief       Does nothing, as output is stateless.
 *
 * @param       hDisplay - pointer to Display_Config struct
 *
 * @return      void
 */
void DisplayUart_clear(Display_Handle hDisplay)
{
}


/*!
 * @fn          DisplayUart_clearLines
 *
 * @brief       Does nothing, as output is stateless.
 *
 * @param       hDisplay - pointer to Display_Config struct
 * @param       lineFrom - line index (0 .. )
 * @param       lineTo - line index (0 .. )
 *
 * @return      void
 */
void DisplayUart_clearLines(Display_Handle hDisplay,
                            uint8_t lineFrom, uint8_t lineTo)
{
}


/*!
 * @fn          DisplayUart_put5
 *
 * @brief       Write a text string to UART with return and newline.
 *
 * @param       hDisplay - pointer to Display_Config struct
 * @param       line - line index (0..)
 * @param       column - column index (0..)
 * @param       fmt - format string
 * @param       aN - optional format arguments
 *
 * @return      void
 */
void DisplayUart_put5(Display_Handle hDisplay, uint8_t line,
                      uint8_t column, uintptr_t fmt, uintptr_t a0,
                      uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4)
{
    DisplayUart_Object  *object  = (DisplayUart_Object  *)hDisplay->object;
    DisplayUart_HWAttrs *hwAttrs = (DisplayUart_HWAttrs *)hDisplay->hwAttrs;

    uint32_t strSize = 0;

    if (Semaphore_pend((Semaphore_Handle) & object->mutex, hwAttrs->mutexTimeout))
    {
        System_snprintf(hwAttrs->strBuf, hwAttrs->strBufLen - 2, (xdc_CString)fmt, a0, a1, a2, a3, a4);

        strSize = strlen(hwAttrs->strBuf);
        hwAttrs->strBuf[strSize++] = '\r';
        hwAttrs->strBuf[strSize++] = '\n';

        UART_write(object->hUart, hwAttrs->strBuf, strSize);
        Semaphore_post((Semaphore_Handle) & object->mutex);
    }
}


/*!
 * @fn          DisplayUart_close
 *
 * @brief       Closes the UART handle
 *
 * @param       hDisplay - pointer to Display_Config struct
 *
 * @return      void
 */
void DisplayUart_close(Display_Handle hDisplay)
{
    DisplayUart_Object *object = (DisplayUart_Object  *)hDisplay->object;

    // Not sure what happens if someone is writing
    UART_close(object->hUart);
    object->hUart = NULL;

    // Not sure what happens if someone is pending
    Semaphore_destruct(&object->mutex);
}

/*!
 * @fn          DisplayUart_control
 *
 * @brief       Function for setting control parameters of the Display driver
 *              after it has been opened.
 *
 * @param       hDisplay - pointer to Display_Config struct
 * @param       cmd - command to execute
 * @param       arg - argument to the command
 *
 * @return      ::DISPLAY_STATUS_UNDEFINEDCMD because no commands are supported
 */
int DisplayUart_control(Display_Handle hDisplay, unsigned int cmd, void *arg)
{
    return DISPLAY_STATUS_UNDEFINEDCMD;
}

/*!
 * @fn          DisplayUart_getType
 *
 * @brief       Returns type of transport
 *
 * @return      Display type UART
 */
unsigned int DisplayUart_getType(void)
{
    return Display_Type_UART;
}
