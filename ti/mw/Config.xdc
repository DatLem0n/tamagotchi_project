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
 *  ======== Config.xdc ========
 */

/*!
 *  ======== Config ========
 */
metaonly module Config {

    /*!
     *  ======== provideDisplayLib ========
     *  Adds the Display middleware library to the project.
     *
     *  This option is only supported on CC13XX & CC26XX devices and the
     *  default value is true.
     *
     */
    metaonly config Bool provideDisplayLib;

    /*!
     *  ======== provideGrLib ========
     *  Adds the GrLib middleware library to the project.
     *
     *  This option is only supported on CC13XX & CC26XX devices and the
     *  default value is true.
     *
     */
    metaonly config Bool provideGrLib;

    /*!
     *  ======== provideLCDLib ========
     *  Adds the LCD middleware library to the project.
     *
     *  This option is only supported on CC13XX & CC26XX devices and the
     *  default value is true.
     *
     */
    metaonly config Bool provideLCDLib;

    /*!
     *  ======== provideWiFiCC3X00Lib ========
     *  Adds the CC3X00 WiFi host driver middleware library & the TI-RTOS WiFi
     *  driver library to the project.
     *
     *  This option is only supported on MSP430, MSP432 & TivaC devices and the
     *  default value is true.
     *
     */
    metaonly config Bool provideWiFiCC3X00Lib;

    /*!
     *  ======== supportsDisplayLib ========
     *  @_nodoc
     */
    metaonly config Bool supportsDisplayLib;

    /*!
     *  ======== supportsGrLib ========
     *  @_nodoc
     */
    metaonly config Bool supportsGrLib;

    /*!
     *  ======== supportsLCDLib ========
     *  @_nodoc
     */
    metaonly config Bool supportsLCDLib;

    /*!
     *  ======== supportsWiFiCC3X00Lib ========
     *  @_nodoc
     */
    metaonly config Bool supportsWiFiCC3X00Lib;
}
