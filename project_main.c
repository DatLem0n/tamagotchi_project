/* C Standard library */
#include <stdio.h>

/* XDCtools files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CCC26XX.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/UART.h>

/* Board Header files */
#include "Board.h"
#include "sensors/opt3001.h" //valoisuus
#include "sensors/mpu9250.h" //kiihtyvyys
#include "sensors/bmp280.h" //lämpötila ja paine

#include "tamagotchi_IO.h"
#include "shared.h"

/*
* Globaalit muuttujat
*/
char messageBuffer[BUFFERSIZE];
char sensorTaskStack[STACKSIZE];
char uartTaskStack[STACKSIZE];
char buzzerTaskStack[STACKSIZE];

float ax, ay, az, gx, gy, gz;
double temp, press, light;
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
static PIN_Handle buzzerHandle;
static PIN_State buzzerState;

// MPU power pin global variables
static PIN_Handle mpuPinHandle;
static PIN_State  mpuPinState;

// MPU power pin
static PIN_Config mpuPinConfig[] = {
    Board_MPU_POWER | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

// MPU uses its own I2C interface
static const I2CCC26XX_I2CPinCfg i2cMPUCfg = {
    .pinSDA = Board_I2C0_SDA1,
    .pinSCL = Board_I2C0_SCL1
};

PIN_Config buttonConfig[] = {
   Board_BUTTON0 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
   PIN_TERMINATE // Asetustaulukko lopetetaan aina tällä vakiolla
};

PIN_Config ledConfig[] = {
   Board_LED0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
   PIN_TERMINATE // Asetustaulukko lopetetaan aina tällä vakiolla
};

PIN_Config buzzerConfig[] = {
  Board_BUZZER | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
  PIN_TERMINATE
};

/*
* Taskit ja nappifunktio
*/

enum Music music_selection = SILENT;
void buttonFxn(PIN_Handle handle, PIN_Id pinId) {

   //TEST
   //eat(1, messageBuffer);

   //Painamalla nappia aloitetaan tai lopetetaan datan lähetys
   if (sensorState == SENSORS_READY) {
      writeMessageBuffer("session:start", messageBuffer);
      sensorState = SENSORS_SENDING_DATA;
   }
   else {
      writeMessageBuffer("session:end", messageBuffer);
      sensorState = SENSORS_READY;
   }
   //System_printf("MessageBuffer:%s\n", messageBuffer);
   //System_flush();
   music_selection++;
   if(music_selection == END)
      music_selection = SILENT;

}

void buzzerTaskFxn(UArg arg0, UArg arg1) {

   while (1) {
      makeSound(buzzerHandle, music_selection);
      //Task_sleep(SECOND/2);
   }
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
         UART_write(uart, messageBuffer, strlen(messageBuffer) + 1);
         strcpy(messageBuffer, '\0');
      }

      // 10x per second
      Task_sleep((SECOND / 10));
   }
}

Void sensorTaskFxn(UArg arg0, UArg arg1) {

   I2C_Handle      i2c_mpu9250, i2c_opt3001, i2c_bmp280;
   I2C_Params      i2cParams_mpu9250, i2cParams_opt3001, i2cParams_bmp280;

   // Alustetaan i2c-väylä
   I2C_Params_init(&i2cParams_mpu9250);
   I2C_Params_init(&i2cParams_opt3001);
   I2C_Params_init(&i2cParams_bmp280);
   i2cParams_mpu9250.bitRate = I2C_400kHz;
   i2cParams_mpu9250.custom = (uintptr_t)&i2cMPUCfg;
   i2cParams_opt3001.bitRate = I2C_400kHz;
   i2cParams_bmp280.bitRate = I2C_400kHz;

   // Alustetaan MPU9250
   i2c_mpu9250 = I2C_open(Board_I2C_TMP, &i2cParams_mpu9250);
   if (i2c_mpu9250 == NULL)
      System_abort("Error Initializing mpu9250 I2C\n");
   PIN_setOutputValue(mpuPinHandle, Board_MPU_POWER, Board_MPU_POWER_ON);
   Task_sleep(SECOND/10);
   mpu9250_setup(&i2c_mpu9250);
   I2C_close(i2c_mpu9250);

   //Alustetaan OPT3001
   i2c_opt3001 = I2C_open(Board_I2C_TMP, &i2cParams_opt3001);
   if (i2c_opt3001 == NULL)
      System_abort("Error Initializing opt3001 I2C\n");
   Task_sleep(SECOND/10);
   opt3001_setup(&i2c_opt3001);
   I2C_close(i2c_opt3001);

   // Alustetaan BMP280
   i2c_bmp280 = I2C_open(Board_I2C_TMP, &i2cParams_bmp280);
   if (i2c_bmp280 == NULL)
      System_abort("Error Initializing mpu9250 I2C\n");
      Task_sleep(SECOND/10);
   bmp280_setup(&i2c_bmp280);
   I2C_close(i2c_opt3001);

   int index = 0;
   while (1) {
      time = Clock_getTicks();

      // Avataan MPU9250 yhteys
      i2c_mpu9250 = I2C_open(Board_I2C_TMP, &i2cParams_mpu9250);
      if (i2c_mpu9250 == NULL)
         System_abort("Error Initializing mpu9250 I2C\n");
      Task_sleep(SECOND/10);
      // Haetaan data
      mpu9250_get_data(&i2c_mpu9250, &ax, &ay, &az, &gx, &gy, &gz);
      // Suljetaan yhteys
      I2C_close(i2c_mpu9250);

      //PIN_setOutputValue(mpuPinHandle,Board_MPU_POWER, Board_MPU_POWER_OFF);

      // Avataan OPT3001 yhteys
      i2c_opt3001 = I2C_open(Board_I2C_TMP, &i2cParams_opt3001);
      if (i2c_opt3001 == NULL)
         System_abort("Error Initializing opt3001 I2C\n");
      Task_sleep(SECOND/10);
      // Haetaan data
      light = opt3001_get_data(&i2c_opt3001);
      // Suljetaan yhteys
      I2C_close(i2c_opt3001);

      // Avataan BMP280 yhteys
      i2c_bmp280 = I2C_open(Board_I2C_TMP, &i2cParams_bmp280);
      if (i2c_bmp280 == NULL)
         System_abort("Error Initializing mpu9250 I2C\n");
      Task_sleep(SECOND/10);
      // Haetaan data
      bmp280_get_data(&i2c_bmp280, &temp, &press);
      // Suljetaan yhteys
      I2C_close(i2c_bmp280);

      // Tallennetaan data sensor_data taulukkoon
      write_mpu9250_to_sensor_data(sensor_data, &index, &ax, &ay, &az, &gx, &gy, &gz);
      write_other_sensors_to_sensor_data(sensor_data, &index, &temp, &press, &light);

      if (sensorState == SENSORS_SENDING_DATA) {
         writeSensorsToMsgBuffer(&messageBuffer, &time, &ax, &ay, &az, &gx, &gy, &gz, &temp, &press, &light);
      }

      index++;
      if (index == SENSOR_DATA_ROWS)
         index = 0;

      // 10x per second, you can modify this
      //Task_sleep(1000000/10);
   }
}


int main(void) {

   // Task variables
   Task_Handle sensorTaskHandle;
   Task_Params sensorTaskParams;
   Task_Handle uartTaskHandle;
   Task_Params uartTaskParams;
   Task_Handle buzzerTaskHandle;
   Task_Params buzzerTaskParams;

   // Initialize board
   Board_initGeneral();


   mpuPinHandle = PIN_open(&mpuPinState, mpuPinConfig);
   if (!mpuPinHandle) {
      System_abort("Error initializing MPU power pin\n");
   }

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

   // Buzzer
   buzzerHandle = PIN_open(&buzzerState, buzzerConfig);
   if (buzzerHandle == NULL) {
      System_abort("Buzzer pin open failed!");
   }

   // Sensor Task
   Task_Params_init(&sensorTaskParams);
   sensorTaskParams.stackSize = STACKSIZE;
   sensorTaskParams.stack = &sensorTaskStack;
   sensorTaskParams.priority = 2;
   sensorTaskHandle = Task_create(sensorTaskFxn, &sensorTaskParams, NULL);
   if (sensorTaskHandle == NULL) {
      System_abort("Task create failed!");
   }

   // UART Task
   Task_Params_init(&uartTaskParams);
   uartTaskParams.stackSize = STACKSIZE;
   uartTaskParams.stack = &uartTaskStack;
   uartTaskParams.priority = 2;
   uartTaskHandle = Task_create(uartTaskFxn, &uartTaskParams, NULL);
   if (uartTaskHandle == NULL) {
      System_abort("Task create failed!");
   }

   // Buzzer Task
   Task_Params_init(&buzzerTaskParams);
   buzzerTaskParams.stackSize = STACKSIZE;
   buzzerTaskParams.stack = &buzzerTaskStack;
   buzzerTaskHandle = Task_create((Task_FuncPtr)buzzerTaskFxn, &buzzerTaskParams, NULL);
   if (buzzerTaskHandle == NULL) {
      System_abort("Buzzer task create failed!");
   }

   /* Sanity check */
   System_printf("Hello world!\n");
   System_flush();

   /* Start BIOS */
   BIOS_start();

   return (0);
}
