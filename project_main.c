/* C Standard library */
#include <stdio.h>

/* XDCtools files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/UART.h>

/* Board Header files */
#include "Board.h"
#include "sensors/opt3001.h" //valoisuus
#include "sensors/mpu9250.h" //kiihtyvyys
#include "sensors/bmp280.h" //lämpötila ja paine

#include "tamagotchi_IO.h"

/* Task */
#define STACKSIZE 2048
#define BUFFERSIZE 80
// TODO: Suurenna jos muistia riittää
#define SENSOR_DATA_ROWS 10
#define SENSOR_DATA_COLUMNS 10

/*
* Globaalit muuttujat
*/
char messageBuffer[BUFFERSIZE];
char sensorTaskStack[STACKSIZE];
char uartTaskStack[STACKSIZE];

float ax, ay, az, gx, gy, gz, temp, press, light;
int time;

float sensor_data[SENSOR_DATA_ROWS][SENSOR_DATA_COLUMNS];
enum SensorDataKeys { TIME, AX, AY, AZ, GX, GY, GZ, TEMP, PRESS, LIGHT };

// JTKJ: Exercise 3. Definition of the state machine
enum SensorState { SENSORS_READY, SENSORS_SENDING_DATA };
enum SensorState sensorState = SENSORS_READY;

// JTKJ: Exercise 1. Add pins RTOS-variables and configuration here
static PIN_Handle buttonHandle;
static PIN_State buttonState;
static PIN_Handle ledHandle;
static PIN_State ledState;

PIN_Config buttonConfig[] = {
   Board_BUTTON0 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
   PIN_TERMINATE // Asetustaulukko lopetetaan aina tällä vakiolla
};

PIN_Config ledConfig[] = {
   Board_LED0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
   PIN_TERMINATE // Asetustaulukko lopetetaan aina tällä vakiolla
};

/*
* Taskit ja nappifunktio
*/

void buttonFxn(PIN_Handle handle, PIN_Id pinId) {

   //TEST
   //Painamalla nappia aloitetaan tai lopetetaan datan lähetys
   if (sensorState == SENSORS_READY) {
      writeMessageBuffer("session:start", messageBuffer);
      sensorState = SENSORS_SENDING_DATA;
   }
   else {
      writeMessageBuffer("session:end", messageBuffer);
      sensorState = SENSORS_READY;
   }
   System_printf("MessageBuffer:%s\n", messageBuffer);
   System_flush();

}

/* Task Functions */
void uartTaskFxn(UArg arg0, UArg arg1) {
   // JTKJ: Exercise 4. Setup here UART connection as 9600,8n1

   // UART-kirjaston asetukset
   UART_Handle uart;
   UART_Params uartParams;

   // Alustetaan sarjaliikenne
   UART_Params_init(&uartParams);
   uartParams.writeDataMode = UART_DATA_TEXT;
   uartParams.readDataMode = UART_DATA_TEXT;
   uartParams.readEcho = UART_ECHO_OFF;
   uartParams.readMode = UART_MODE_BLOCKING;
   uartParams.baudRate = 9600; // nopeus 9600baud
   uartParams.dataLength = UART_LEN_8; // 8
   uartParams.parityType = UART_PAR_NONE; // n
   uartParams.stopBits = UART_STOP_ONE; // 1

   // Avataan yhteys laitteen sarjaporttiin vakiossa Board_UART0
   uart = UART_open(Board_UART0, &uartParams);
   if (uart == NULL) {
      System_abort("Error opening the UART");
   }

   while (1) {

      //Jos viestibufferissa on dataa, lähetetään se ja nollataan bufferi
      if (messageBuffer[0] != '\0') {
         UART_write(uart, messageBuffer, strlen(messageBuffer));
         strcpy(messageBuffer, "");
      }

      // 4x per second
      Task_sleep((1000000 / 4) / Clock_tickPeriod);
   }
}

Void sensorTaskFxn(UArg arg0, UArg arg1) {

   I2C_Handle      i2c;
   I2C_Params      i2cParams;

   // Alustetaan i2c-väylä
   I2C_Params_init(&i2cParams);
   i2cParams.bitRate = I2C_400kHz;

   // Avataan yhteys
   i2c = I2C_open(Board_I2C_TMP, &i2cParams);
   if (i2c == NULL) {
      System_abort("Error Initializing I2C\n");
   }

   // JTKJ: Teht�v� 2. Alusta sensorin OPT3001 setup-funktiolla
   //       Laita enne funktiokutsua eteen 100ms viive (Task_sleep)
   Task_sleep(100000 / Clock_tickPeriod);

   //TODO: lisää sensors-kansion tiedostoihin puuttuvat rekisteripyörittelyt
   //TODO: alusta loput sensorit
   opt3001_setup(&i2c);
   mpu9250_setup(&i2c);
   bmp280_setup(&i2c);

   short index = 0;
   while (1) {
      //TODO: lisää loput sensorit
      time = Clock_getTicks();
      light = opt3001_get_data(&i2c);
      bmp280_get_data(&i2c, temp, press);
      mpu9250_get_data(&i2c, ax, ay, az, gx, gy, gz);

      write_mpu9250_to_sensor_data(sensor_data, index, ax, ay, az, gx, gy, gz);
      write_other_sensors_to_sensor_data(sensor_data, index, temp, press, light);

      if (sensorState == SENSORS_SENDING_DATA) {
         write_sensor_data_to_messageBuffer(messageBuffer, time, ax, ay, az, gx, gy, gz, temp, press, light);
      }

      index++;
      if (index == SENSOR_DATA_ROWS)
         index = 0;
      // Once per second, you can modify this
      Task_sleep(1000000 / Clock_tickPeriod);
   }
}


int main(void) {

   // Task variables
   Task_Handle sensorTaskHandle;
   Task_Params sensorTaskParams;
   Task_Handle uartTaskHandle;
   Task_Params uartTaskParams;

   // Initialize board
   Board_initGeneral();

   // JTKJ: Exercise 1. Open the button and led pins
   //       Remember to register the above interrupt handler for button
   // Ledi käyttöön ohjelmassa
   ledHandle = PIN_open(&ledState, ledConfig);
   if (!ledHandle) {
      System_abort("Error initializing LED pin\n");
   }

   // Painonappi käyttöön ohjelmassa
   buttonHandle = PIN_open(&buttonState, buttonConfig);
   if (!buttonHandle) {
      System_abort("Error initializing button pin\n");
   }

   // Painonapille keskeytyksen käsittellijä
   if (PIN_registerIntCb(buttonHandle, &buttonFxn) != 0) {
      System_abort("Error registering button callback function");
   }



   /* Task */
   Task_Params_init(&sensorTaskParams);
   sensorTaskParams.stackSize = STACKSIZE;
   sensorTaskParams.stack = &sensorTaskStack;
   sensorTaskParams.priority = 2;
   sensorTaskHandle = Task_create(sensorTaskFxn, &sensorTaskParams, NULL);
   if (sensorTaskHandle == NULL) {
      System_abort("Task create failed!");
   }

   Task_Params_init(&uartTaskParams);
   uartTaskParams.stackSize = STACKSIZE;
   uartTaskParams.stack = &uartTaskStack;
   uartTaskParams.priority = 2;
   uartTaskHandle = Task_create(uartTaskFxn, &uartTaskParams, NULL);
   if (uartTaskHandle == NULL) {
      System_abort("Task create failed!");
   }

   /* Sanity check */
   System_printf("Hello world!\n");
   System_flush();

   /* Start BIOS */
   BIOS_start();

   return (0);
}
