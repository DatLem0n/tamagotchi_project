/*
 * tmp007.c
 *
 *  Created on: 28.9.2016
 *  Author: Teemu Leppanen / UBIComp / University of Oulu
 *
 *  Datakirja: http://www.ti.com/lit/ds/symlink/tmp007.pdf
 */

#include <xdc/runtime/System.h>
#include <string.h>
#include "Board.h"
#include "tmp007.h"
#include <stdint.h>

void tmp007_setup(I2C_Handle *i2c) {

	System_printf("TMP007: Config OK!\n");
    System_flush();
}

/**************** JTKJ: DO NOT MODIFY ANYTHING ABOVE THIS LINE ****************/

double tmp007_get_data(I2C_Handle *i2c) {
    double tempMultiplier = 0.03125;
	double temperature = 0.0; // return value of the function

    char txBuffer[1];
    char rxBuffer[2];

    I2C_Transaction i2cMessage;

    i2cMessage.slaveAddress = Board_TMP007_ADDR;
    txBuffer[0] = TMP007_REG_TEMP;
    i2cMessage.writeBuf = txBuffer;
    i2cMessage.writeCount = 1;
    i2cMessage.readBuf = rxBuffer;
    i2cMessage.readCount = 2;

	if (I2C_transfer(*i2c, &i2cMessage)) {
        uint16_t rekisteri = rxBuffer[0] << 8 | rxBuffer[1];
        uint16_t data = rekisteri >> 2;
        temperature = data * tempMultiplier;

	} else {

		System_printf("TMP007: Data read failed!\n");
		System_flush();
	}

	return temperature;
}
