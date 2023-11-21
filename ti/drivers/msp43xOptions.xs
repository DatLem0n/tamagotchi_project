/*
 *  ======== msp43xOptions.xs ========
 */

/*
 *  ======== getCompilerLinkerOptions ========
 *  Return compiler paths and link libraries for a given device.
 */
function getCompilerLinkerOptions(part, mspWareOpts, tool)
{
    if (part.match(/MSP432P4*/g)) {
        return (getMSP432Options(part, mspWareOpts, tool));
    }
    else if (part.match(/MSP430F*/g)) {
        return (getMSP430Options(part, mspWareOpts, tool));
    }
    else {
        throw ("Invalid part supplied: '" + part + "'");
    }
}

/*
 *  ======== getMSP430Options ========
 *  Return compiler paths and link libraries for MSP430 devices.
 */
function getMSP430Options(part, mspWareOpts, tool) {
    var compilerOptions = "";
    var linkerOptions = "";
    var driverLib = mspWareOpts["msp430DriverLib"];
    var headers = mspWareOpts["msp430Headers"];
    var iarHeaders = mspWareOpts["iarMSP430Headers"];

    /* Add tool specific search directories & driverlib libraries */
    if (tool == "TI") {
        if (part.match(/MSP430FR[56]\d\w]*/g)) {
            linkerOptions = " -l" + driverLib +
                "/driverlib/MSP430FR5xx_6xx/ccs/" + part + ".lib";
        }
        else if (part.match(/MSP430F[56][\d\w]*/g)) {
            linkerOptions = " -l" + driverLib +
                "/driverlib/MSP430F5xx_6xx/ccs/" + part + ".lib";
        }

        if (headers) {
            compilerOptions = " -I" + headers;
            linkerOptions += " -i" + headers;
        }
    }
    else if (tool == "IAR") {
        if (iarHeaders) {
            compilerOptions = " -I" + iarHeaders + " -I" + iarHeaders + "/dlib/c";
        }

        if (part.match(/MSP430FR[56]\d\w]*/g)) {
            linkerOptions = " -C " + driverLib +
                "/driverlib/MSP430FR5xx_6xx/iar/" + part + ".r43";
        }
        else if (part.match(/MSP430F[56][\d\w]*/g)) {
            linkerOptions = " -C " + driverLib +
                "/driverlib/MSP430F5xx_6xx/iar/" + part + ".r43";
        }
    }
    else {
        throw ("Unsupported tool-chain: '" + tool + "'");
    }

    /* Add driverlib include paths & device name */
    if (part.match(/MSP430FR[56]\d\w]*/g)) {
        compilerOptions += " -I" + driverLib + "/driverlib/MSP430FR5xx_6xx";
    }
    else if (part.match(/MSP430F[56][\d\w]*/g)) {
        compilerOptions += " -I" + driverLib + "/driverlib/MSP430F5xx_6xx";
    }
    else {
        throw("Invalid part supplied: '" + part + "'");
    }

    compilerOptions += " -D__" + part + "__";

    return ({cOpts: compilerOptions, lOpts: linkerOptions});
}

/*
 *  ======== getMSP432Options ========
 *  Return compiler paths and link libraries for MSP432S devices.
 */
function getMSP432Options(part, mspWareOpts, tool) {
    var compilerOptions = "";
    var likerOptions = "";
    var driverLib = mspWareOpts["msp432DriverLib"];
    var iarHeaders = mspWareOpts["iarMSP432Headers"];
    var driverlibPath = driverLib + "/driverlib/MSP432P4xx"

    /* Add tool specific search directories & driverlib libraries */
    if (tool == "IAR") {
        if (iarHeaders) {
            compilerOptions += " -I" + iarHeaders + "/../c";
        }
        likerOptions = " " + driverlibPath + "/ewarm/msp432p4xx_driverlib.a";
    }
    else if (tool == "TI") {
        likerOptions = " -l" + driverlibPath + "/ccs/msp432p4xx_driverlib.lib";
    }
    else if (tool == "GNU") {
        likerOptions = " -L" + driverlibPath + "/gcc -l:msp432p4xx_driverlib.a"
    }
    else {
        throw ("Unsupported tool-chain: '" + tool + "'");
    }

    /* Add driverlib include paths & device name */
    compilerOptions += " -I" + driverLib + "/inc/CMSIS" +
        " -I" + driverLib + "/inc" +
        " -I" + driverlibPath + " -D__" + part + "__";

    return ({cOpts: compilerOptions, lOpts: likerOptions});
}
