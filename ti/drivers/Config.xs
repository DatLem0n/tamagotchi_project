/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
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
 *  ======== Config.xs ========
 */

var ClockFreqs;
var driversConfig;
var Program;

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    driversConfig = xdc.module('ti.drivers.Config');
    Program = xdc.module('xdc.cfg.Program');

    /* Prevent false failure when building the module. */
    if (Program.cpu) {
        determineSupport();
    }
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    /* Add driver dependencies & requirements */
    Program = xdc.module('xdc.cfg.Program');

    /* Add ports library */
    xdc.loadPackage('ti.drivers.ports');

    /*
     * Power driver (ti.drivers.Power) is supported on CC13xx, CC26xx,
     * CC3200 & MSP432
     */
    if ((Program.cpu.deviceName.match(/CC32/)) ||
        (Program.cpu.deviceName.match(/CC13/)) ||
        (Program.cpu.deviceName.match(/CC26/)) ||
        (Program.cpu.deviceName.match(/MSP432/))) {
        xdc.useModule('ti.drivers.Power');
    }

    if (Program.cpu.deviceName.match(/CC13/) ||
        Program.cpu.deviceName.match(/CC26/)) {
        xdc.loadPackage("ti.drivers.pdm");
        xdc.loadPackage("ti.drivers.rf");
    }

    /* Add ClockFreqs module to MSP430 & MSP432 */
    if (Program.cpu.deviceName.match(/MSP432/)) {
        ClockFreqs = xdc.useModule('ti.sysbios.family.arm.msp432.ClockFreqs');
    }
    if (Program.cpu.deviceName.match(/MSP430/)) {
        ClockFreqs = xdc.useModule('ti.sysbios.family.msp430.ClockFreqs');
    }
}

/*
 *  ======== determineSupport ========
 */
function determineSupport()
{
    if (Program.cpu.deviceName.match(/CC[12].*/)) {
        driversConfig.supportsRFDriver = true;
    }
    else {
        driversConfig.supportsRFDriver = false;
    }
}
