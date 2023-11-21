/*
 *  ======== xdcArgParser.xs ========
 */

/*
 *  ======== getDeviceWareOpts========
 *  Parses XDCARGS returning an object with paths to device *wares,
 *  MSP430/MSP432 header file paths and MSP430/MSP432 device lists.
 */
function getDeviceWareOpts(xdcArgs)
{
    if (typeof xdcArgs == "undefined" || xdcArgs == "") {
        throw "Products must be specified in XDCARGS";
    }

    /* Get paths from XDCARGS options */
    var argMap = parseArgs(xdcArgs);

    return ({
        "cc13xxware": argMap["cc13xxware"],
        "cc26xxware": argMap["cc26xxware"],
        "cc32xxware": argMap["cc32xxware"],
        "msp430DriverLib": argMap["msp430DriverLib"],
        "msp432DriverLib": argMap["msp432DriverLib"],
        "msp430GrLib": argMap["msp430Grlib"],
        "msp430UsbLib": argMap["msp430Usblib"],
        "mware": argMap["mware"],
        "tivaware": argMap["tivaware"],
        "msp430Headers": argMap["msp430Headers"],
        "msp432Headers": argMap["msp432Headers"],
        "iarMSP430Headers": argMap["iarMSP430Headers"],
        "iarMSP432Headers": argMap["iarMSP432Headers"],
        "msp430DevList": argMap["msp430DevList"],
        "msp432DevList": argMap["msp432DevList"]
    });
}

/* Returns a JSON map object of XDCARGS */
function parseArgs(xdcArgs) {
    var argMap = {};
    var msp430DevList = [];
    var msp432DevList = [];

    /* Parse product versions from XDCARGS options */
    for each (var arg in xdcArgs.split(" ")) {
        if (arg.split('=').length < 2) {
            /* skip undefined arguments */
            continue;
        }

        if (arg.match(/^MCPI=/)) {
            argMap["mcpi"] = String(arg.split("=").pop());
        }

        if (arg.match(/^XDCTOOLS=/)) {
            argMap["xdctools"] = String(arg.split("=").pop());
        }

        if (arg.match(/^TIRTOS=/)) {
            argMap["tirtos"] = String(arg.split("=").pop());
        }

        if (arg.match(/^TIDRIVERS=/)) {
            argMap["tidrivers"] = String(arg.split("=").pop());
        }

        if (arg.match(/^BIOS=/)) {
            argMap["bios"] = String(arg.split("=").pop());
        }

        if (arg.match(/^NDK=/)) {
            argMap["ndk"] = String(arg.split("=").pop());
        }

        if (arg.match(/^NS=/)) {
            argMap["ns"] = String(arg.split("=").pop());
        }

        if (arg.match(/^UIA=/)) {
            argMap["uia"] = String(arg.split("=").pop());
        }

        if (arg.match(/^CC13XXWARE=/)) {
            argMap["cc13xxware"] = String(arg.split("=").pop());
        }

        if (arg.match(/^CC26XXWARE=/)) {
            argMap["cc26xxware"] = String(arg.split("=").pop());
        }

        if (arg.match(/^CC32XXWARE=/)) {
            argMap["cc32xxware"] = String(arg.split("=").pop());
        }

        if (arg.match(/^MSP430DRIVERLIB=/)) {
            argMap["msp430DriverLib"] = String(arg.split("=").pop());
        }

        if (arg.match(/^MSP432DRIVERLIB=/)) {
            argMap["msp432DriverLib"] = String(arg.split("=").pop());
        }

        if (arg.match(/^MSP430GRLIB=/)) {
            argMap["msp430GrLib"] = String(arg.split("=").pop());
        }

        if (arg.match(/^MSP430USBLIB=/)) {
            argMap["msp430UsbLib"] = String(arg.split("=").pop());
        }

        if (arg.match(/^MWARE=/)) {
            argMap["mware"] = String(arg.split("=").pop());
        }

        if (arg.match(/^TIVAWARE=/)) {
            argMap["tivaware"] = String(arg.split("=").pop());
        }

        if (arg.match(/^MSP430HEADERS=/)) {
            argMap["msp430Headers"] = String(arg.split("=").pop());
        }

        if (arg.match(/^MSP432HEADERS=/)) {
            argMap["msp432Headers"] = String(arg.split("=").pop());
        }

        if (arg.match(/^IAR_MSP430HEADERS=/)) {
            argMap["iarMSP430Headers"] = String(arg.split("=").pop());
        }

        if (arg.match(/^IAR_MSP432HEADERS=/)) {
            argMap["iarMSP432Headers"] = String(arg.split("=").pop());
        }

        if (arg.match(/MSP430DEVLIST=/)) {
            argMap["msp430DevList"] = String(arg.split("=").pop()).split(",");
        }

        if (arg.match(/MSP432DEVLIST=/)) {
            argMap["msp432DevList"] = String(arg.split("=").pop()).split(",");
        }
    }

    return argMap;
}
