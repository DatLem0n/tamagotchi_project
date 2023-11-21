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

var driversConfig;
var mwConfig;
var Program;

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    driversConfig = xdc.module('ti.drivers.Config');
    mwConfig = xdc.module('ti.mw.Config');
    Program = xdc.module('xdc.cfg.Program');

    /* Prevent false failure when building the module. */
    if (Program.cpu) {
        determineSupport();
    }
}

function module$use()
{
    if (mwConfig.provideDisplayLib) {
        if (Program.cpu.deviceName.match(/CC[12].*/)) {
            xdc.loadPackage('ti.mw.display');
            xdc.loadPackage('ti.mw.grlib');
            xdc.loadPackage('ti.mw.lcd');
        }
        else {
            mwConfig.$logError("The Display middleware library is not supported on: " +
                Program.cpu.deviceName + ".  The library will not be added to the project.", mwConfig);
        }
    }

    if (mwConfig.provideGrLib) {
        if (Program.cpu.deviceName.match(/CC[12].*/)) {
            xdc.loadPackage('ti.mw.grlib');
        }
        else {
            mwConfig.$logError("The GrLib middleware library is not supported on: " +
                Program.cpu.deviceName + ".  The library will not be added to the project.", mwConfig);
        }
    }

    if (mwConfig.provideLCDLib) {
        if (Program.cpu.deviceName.match(/CC[12].*/)) {
            xdc.loadPackage('ti.mw.lcd');
        }
        else {
            mwConfig.$logError("The LCD middleware library is not supported on: " +
                Program.cpu.deviceName + ".  The library will not be added to the project.", mwConfig);
        }
    }

    if (mwConfig.provideWiFiCC3X00Lib) {
        if (Program.cpu.deviceName.match(/CC320.*/) ||
            Program.cpu.deviceName.match(/MSP430.*/) ||
            Program.cpu.deviceName.match(/MSP432.*/) ||
            Program.cpu.deviceName.match(/TM4[CE].*/)) {
            driversConfig.useWiFiDriver = true;
            xdc.loadPackage('ti.mw.wifi.cc3x00');
        }
        else {
            mwConfig.$logError("CC3X00 WiFi not supported on: " +
                Program.cpu.deviceName + ".  The library will not be added to the project.", mwConfig);
            driversConfig.useWiFiDriver = false;
        }
    }
    else {
        driversConfig.useWiFiDriver = false;
    }
}

/*
 *  ======== determineSupport ========
 */
function determineSupport()
{
    if (Program.cpu.deviceName.match(/CC[12].*/)) {
        mwConfig.provideDisplayLib = true;
        mwConfig.supportsDisplayLib = true;

        mwConfig.provideGrLib = true;
        mwConfig.supportsGrLib = true;

        mwConfig.provideLCDLib = true;
        mwConfig.supportsLCDLib = true;
    }
    else if (Program.cpu.deviceName.match(/CC320.*/)) {
        mwConfig.provideWiFiCC3X00Lib = true;
        mwConfig.supportsWiFiCC3X00Lib = true;
    }
    else if (Program.cpu.deviceName.match(/MSP430.*/)) {
        mwConfig.provideWiFiCC3X00Lib = true;
        mwConfig.supportsWiFiCC3X00Lib = true;
    }
    else if (Program.cpu.deviceName.match(/MSP432.*/)) {
        mwConfig.provideWiFiCC3X00Lib = true;
        mwConfig.supportsWiFiCC3X00Lib = true;
    }
    else if (Program.cpu.deviceName.match(/TM4[CE].*/)) {
        mwConfig.provideWiFiCC3X00Lib = true;
        mwConfig.supportsWiFiCC3X00Lib = true;
    }
    else {}
}
