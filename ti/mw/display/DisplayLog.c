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
/* In case the lib is built as non-instrumented, force Log enable here */
#if defined(xdc_runtime_Log_DISABLE_ALL)
#undef xdc_runtime_Log_DISABLE_ALL
#endif
#include <xdc/runtime/Log.h>   // For Log_warning1("Warning number #%d", 4); things
#include <xdc/runtime/Diags.h> // For Log_print0(Diags_USER1, "hello"); things.

#include <ti/mw/display/Display.h>
#include <ti/mw/display/DisplayLog.h>

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* -----------------------------------------------------------------------------
 *  Constants and macros
 * -----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------
 *   Type definitions
 * -----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------
 *                           Local variables
 * ------------------------------------------------------------------------------
 */
/* Display function table for DisplayLog implementation */
const Display_FxnTable DisplayLog_fxnTable = {
    DisplayLog_open,
    DisplayLog_clear,
    DisplayLog_clearLines,
    DisplayLog_put5,
    DisplayLog_close,
    DisplayLog_control,
    DisplayLog_getType,
};

/* -----------------------------------------------------------------------------
 *                                          Functions
 * -----------------------------------------------------------------------------
 */
/*!
 * @fn          DisplayLog_open
 *
 * @brief       Do nothing - the Log output is configured elsewhere.
 *
 * @param       hDisplay - pointer to Display_Config struct
 * @param       params - display parameters
 *
 * @return      Pointer to Display_Config struct
 */
Display_Handle DisplayLog_open(Display_Handle handle,
                               Display_Params *params)
{
    return handle;
}


/*!
 * @fn          DisplayLog_clear
 *
 * @brief       Does nothing, as the log is stateless and can't be cleared.
 *
 * @param       hDisplay - pointer to Display_Config struct
 *
 * @return      void
 */
void DisplayLog_clear(Display_Handle handle)
{
}


/*!
 * @fn          DisplayLog_clearLines
 *
 * @brief       Does nothing, as the log is stateless and can't be cleared.
 *
 * @param       hDisplay - pointer to Display_Config struct
 * @param       lineFrom - line index (0 .. )
 * @param       lineTo - line index (0 .. )
 *
 * @return      void
 */
void DisplayLog_clearLines(Display_Handle handle, uint8_t fromLine,
                           uint8_t toLine)
{
}


/*!
 * @fn          DisplayLog_put5
 *
 * @brief       Log_print an event to USER6 formatted as "DISPLAY: ..."
 *
 * @param       hDisplay - pointer to Display_Config struct
 * @param       line - line index (0..)
 * @param       column - column index (0..)
 * @param       fmt - format string
 * @param       aN - optional format arguments
 *
 * @return      void
 */
void DisplayLog_put5(Display_Handle handle, uint8_t line, uint8_t column,
                     uintptr_t fmt, uintptr_t a0, uintptr_t a1,
                     uintptr_t a2, uintptr_t a3, uintptr_t a4)
{
    Log_print6(Diags_USER6, "DISPLAY: %$S", fmt, a0, a1, a2, a3, a4);
}


/*!
 * @fn          DisplayLog_close
 *
 * @brief       Does nothing, closing Log must be done via Log system
 *
 * @param       hDisplay - pointer to Display_Config struct
 *
 * @return      void
 */
void DisplayLog_close(Display_Handle handle)
{
}

/*!
 * @fn          DisplayLog_control
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
int DisplayLog_control(Display_Handle handle, unsigned int cmd, void *arg)
{
    return DISPLAY_STATUS_UNDEFINEDCMD;
}

/*!
 * @fn          DisplayLog_getType
 *
 * @brief       Returns type of transport
 *
 * @return      Display type
 */
unsigned int DisplayLog_getType(void)
{
    return Display_Type_LOG;
}
