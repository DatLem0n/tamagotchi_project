/* C Standard library */
#include <stdio.h>
#include <stdbool.h>
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

/**
 * format for data (columns)
 * TIME   | AX | AY | AZ | GX | GY | GZ | TEMP | PRESS | LIGHT
 * time x | ... |
 * time y | ... |
 *
 * e.g. sensorData [1,TIME] would return *time y*
 */
float sensorDataArray[SENSOR_DATA_ROWS][SENSOR_DATA_COLUMNS];

// "Tilakone" sensoridatan lähetykseen backendille
bool sendSensorDataToBackend = FALSE;

// Pin RTOS-variables and configurations
static PIN_Handle button0_Handle;
static PIN_Handle button1_Handle;
static PIN_State button0_State;
static PIN_State button1_State;
static PIN_Handle ledHandle;
static PIN_State ledState;
static PIN_Handle buzzerHandle;
static PIN_State buzzerState;

// MPU power pin global variables
static PIN_Handle mpuPinHandle;
static PIN_State  mpuPinState;

/*
* Initialize Pin configs here
*/

// MPU power pin
static PIN_Config mpuPowerPinConfig[] = {
    Board_MPU_POWER | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

// MPU uses its own I2C interface
static const I2CCC26XX_I2CPinCfg mpuI2cPinConfig = {
    .pinSDA = Board_I2C0_SDA1,
    .pinSCL = Board_I2C0_SCL1
};

PIN_Config button0_Config[] = {
   Board_BUTTON0 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
   PIN_TERMINATE // Asetustaulukko lopetetaan aina tällä vakiolla
};

PIN_Config button1_Config[] = {
        Board_BUTTON1 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
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
*  Button functions
*/

bool eatButtonPressed = FALSE;
enum Music music_selection = SILENT;
void button0_Fxn(PIN_Handle handle, PIN_Id pinId) {
   music_selection++;
   if (music_selection == END)
      music_selection = SILENT;
}

void button1_Fxn(PIN_Handle handle, PIN_Id pinId) {
   eat(1, messageBuffer);
   eatButtonPressed = TRUE;
   toggleLed(ledHandle, Board_LED0);
}

void buzzerTaskFxn() {
   while (1) {
      if (eatButtonPressed) {
         makeSound(buzzerHandle, EAT);
         eatButtonPressed = FALSE;
      }
      else {
         makeSound(buzzerHandle, music_selection);
      }

      //Task_sleep(SECOND/2);
   }
}

/*
*  Task functions
*/

void uartTaskFxn() {
   UART_Handle uartHandle;
   UART_Params uartParams;

   UART_Params_init(&uartParams);
   uartParams.writeDataMode = UART_DATA_TEXT;
   uartParams.readDataMode = UART_DATA_TEXT;
   uartParams.readEcho = UART_ECHO_OFF;
   uartParams.readMode = UART_MODE_BLOCKING;
   uartParams.baudRate = 9600; // nopeus 9600baud
   uartParams.dataLength = UART_LEN_8; // 8
   uartParams.parityType = UART_PAR_NONE; // n
   uartParams.stopBits = UART_STOP_ONE; // 1

   uartHandle = UART_open(Board_UART0, &uartParams);
   if (uartHandle == NULL) {
      System_abort("Error opening the UART");
   }

   while (1) {

      // TODO: korvaa tilakoneella
      //Jos viestibufferissa on dataa, lähetetään se ja nollataan bufferi
      if (messageBuffer[0] != '\0') {
         UART_write(uartHandle, messageBuffer, strlen(messageBuffer) + 1);
         strcpy(messageBuffer, '\0');
      }

      // 10x per second
      Task_sleep((SECOND / 50));
   }
}

// TODO: bmp280 palauttaa aina samat arvot, plz fix
// TODO: ota käyttöön tmp007
// TODO: poista sleepit, testaa ja laita takaisin ne mikä eivät olleet turhia
Void sensorTaskFxn() {
   I2C_Handle i2c_mpu9250, i2c_opt3001, i2c_bmp280;
   I2C_Params i2cParams_mpu9250, i2cParams_opt3001, i2cParams_bmp280;

   sensorSetup(&i2c_mpu9250, &i2c_opt3001, &i2c_bmp280, &i2cParams_mpu9250, &i2cParams_opt3001, &i2cParams_bmp280);

   int index = 0;
   while (1) {
      time = Clock_getTicks() / Clock_tickPeriod;
      time = time / 1000;

      /*
      * Get data from sensors
      */
      i2c_mpu9250 = I2C_open(Board_I2C_TMP, &i2cParams_mpu9250);
      if (i2c_mpu9250 == NULL)
         System_abort("Error Initializing mpu9250 I2C\n");
      Task_sleep(SECOND / 10);

      mpu9250_get_data(&i2c_mpu9250, &ax, &ay, &az, &gx, &gy, &gz);

      I2C_close(i2c_mpu9250);
      Task_sleep(SECOND / 10);

      // OPT3001
      i2c_opt3001 = I2C_open(Board_I2C_TMP, &i2cParams_opt3001);
      if (i2c_opt3001 == NULL)
         System_abort("Error Initializing opt3001 I2C\n");
      Task_sleep(SECOND);

      light = opt3001_get_data(&i2c_opt3001);

      I2C_close(i2c_opt3001);
      Task_sleep(SECOND / 10);

      // BMP280
      i2c_bmp280 = I2C_open(Board_I2C_TMP, &i2cParams_bmp280);
      if (i2c_bmp280 == NULL)
         System_abort("Error Initializing mpu9250 I2C\n");
      Task_sleep(SECOND / 10);

      bmp280_get_data(&i2c_bmp280, &temp, &press);

      I2C_close(i2c_bmp280);
      Task_sleep(SECOND / 10);

      /*
      * Clean sensor data, write it to sensor_data_array and send it to backend
      */
      clean_mpu9250_data(&ax, &ay, &az, &gx, &gy, &gz);
      //TODO: toteuta clean_other_data (testaa ja keksi sopivat rajat)

      write_sensor_readings_to_sensorDataArray(sensorDataArray, index, time, ax, ay, az, gx, gy, gz, temp, press, light);

      if (sendSensorDataToBackend == TRUE) {
         write_sensor_readings_to_messageBuffer(messageBuffer, time, ax, ay, az, gx, gy, gz, temp, press, light);
      }

      // Used to turn the sensor data array into a ring buffer
      index++;
      if (index == SENSOR_DATA_ROWS)
         index = 0;
   }
}

int main(void) {

   Task_Handle sensorTaskHandle;
   Task_Params sensorTaskParams;
   Task_Handle uartTaskHandle;
   Task_Params uartTaskParams;
   Task_Handle buzzerTaskHandle;
   Task_Params buzzerTaskParams;

   Board_initGeneral();
   initialize_handles();

   initialize_task(&sensorTaskHandle, &sensorTaskParams, &sensorTaskFxn, &sensorTaskStack, 2);
   initialize_task(&uartTaskHandle, &uartTaskParams, &uartTaskFxn, &uartTaskStack, 2);
   initialize_task(&buzzerTaskHandle, &buzzerTaskParams, (Task_FuncPtr)&buzzerTaskFxn, &buzzerTaskStack, 0);

   /* Sanity check */
   System_printf("Hello world!\n");
   System_flush();

   /* Start BIOS */
   BIOS_start();

   return (0);
}

/*
* Initialize pins and handles
*/
void initialize_handles() {
   mpuPinHandle = PIN_open(&mpuPinState, mpuPowerPinConfig);
   if (!mpuPinHandle) {
      System_abort("Error initializing MPU power pin\n");
   }

   // Ledi käyttöön ohjelmassa
   ledHandle = PIN_open(&ledState, ledConfig);
   if (!ledHandle) {
      System_abort("Error initializing LED pin\n");
   }

   // Painonappi 0 käyttöön ohjelmassa
   button0_Handle = PIN_open(&button0_State, button0_Config);
   if (!button0_Handle) {
      System_abort("Error initializing button0 pin\n");
   }
   // Painonappi 1 käyttöön ohjelmassa
   button1_Handle = PIN_open(&button1_State, button1_Config);
   if (!button1_Handle) {
      System_abort("Error initializing button1 pin\n");
   }

   // Painonapille 0 keskeytyksen käsittellijä
   if (PIN_registerIntCb(button0_Handle, &button0_Fxn) != 0) {
      System_abort("Error registering button callback function");
   }

   // Painonapille 1 keskeytyksen käsittellijä
   if (PIN_registerIntCb(button1_Handle, &button1_Fxn) != 0) {
      System_abort("Error registering button callback function");
   }

   // Buzzer
   buzzerHandle = PIN_open(&buzzerState, buzzerConfig);
   if (buzzerHandle == NULL) {
      System_abort("Buzzer pin open failed!");
   }
}

void initialize_task(I2C_Handle* handle, I2C_Params* params, void(*taskFxn), char* stack, uint8_t priority) {
   Task_Params_init(params);
   (*params).stackSize = STACKSIZE;
   (*params).stack = &stack;
   (*handle) = Task_create(taskFxn, &params, NULL);

   if (handle == NULL) {
      System_abort("Task create failed!");
   }
   if (priority != 0)
      (*params).priority = priority;
}

void sensorSetup(I2C_Handle* i2c_mpu9250, I2C_Handle* i2c_opt3001, I2C_Handle* i2c_bmp280,
   I2C_Params* i2cParams_mpu9250, I2C_Params* i2cParams_opt3001, I2C_Params* i2cParams_bmp280) {
   // Alustetaan i2c-väylä
   I2C_Params_init(i2cParams_mpu9250);
   I2C_Params_init(i2cParams_opt3001);
   I2C_Params_init(i2cParams_bmp280);
   (*i2cParams_mpu9250).bitRate = I2C_400kHz;
   (*i2cParams_mpu9250).custom = (uintptr_t)&mpuI2cPinConfig;
   (*i2cParams_opt3001).bitRate = I2C_400kHz;
   (*i2cParams_bmp280).bitRate = I2C_400kHz;

   // Alustetaan MPU9250
   (*i2c_mpu9250) = I2C_open(Board_I2C_TMP, i2cParams_mpu9250);
   if ((*i2c_mpu9250) == NULL)
      System_abort("Error Initializing mpu9250 I2C\n");
   PIN_setOutputValue(mpuPinHandle, Board_MPU_POWER, Board_MPU_POWER_ON);
   Task_sleep(SECOND / 10);
   mpu9250_setup(i2c_mpu9250);
   I2C_close((*i2c_mpu9250));

   //Alustetaan OPT3001
   (*i2c_opt3001) = I2C_open(Board_I2C_TMP, i2cParams_opt3001);
   if ((*i2c_opt3001) == NULL)
      System_abort("Error Initializing opt3001 I2C\n");
   Task_sleep(SECOND / 10);
   opt3001_setup(i2c_opt3001);
   I2C_close((*i2c_opt3001));

   // Alustetaan BMP280
   (*i2c_bmp280) = I2C_open(Board_I2C_TMP, i2cParams_bmp280);
   if ((*i2c_bmp280) == NULL)
      System_abort("Error Initializing mpu9250 I2C\n");
   Task_sleep(SECOND / 10);
   bmp280_setup(i2c_bmp280);
   I2C_close((*i2c_opt3001));
}

