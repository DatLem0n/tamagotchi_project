/* C Standard library */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
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
#include "sensors/bmp280.h"  //lämpötila ja paine

#include "tamagotchi_IO.h"
#include "shared.h"

/* prototypes */
void sensorSetup(I2C_Handle *i2c_mpu9250, I2C_Handle *i2c_opt3001, I2C_Handle *i2c_bmp280,
                 I2C_Params *i2cParams_mpu9250, I2C_Params *i2cParams_opt3001, I2C_Params *i2cParams_bmp280);
void initialize_task(Task_Handle *handle, Task_Params *params, void(*taskFxn), char *stack, uint8_t priority);
void initialize_handles();
void detectPets();

/*
 * Globaalit muuttujat
 */
char messageBuffer[BUFFERSIZE];
char receiveBuffer[BUFFERSIZE];

char sensorTaskStack[STACKSIZE];
char uartTaskStack[STACKSIZE];
char buzzerTaskStack[STACKSIZE];

float ax, ay, az, gx, gy, gz;
int sensorArrayHEAD = 0;

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
float mpu9250DeltasArray[6];

// Sensoridatan lähetykseen backendille
bool sendSensorDataToBackend = FALSE;
int timesSenttoBackend = 0;

bool bouncingDetected = FALSE;
int inDistress = 0;

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
static PIN_State mpuPinState;

/*
 * Initialize Pin configs here
 */

// MPU power pin
static PIN_Config mpuPowerPinConfig[] = {
    Board_MPU_POWER | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE};

// MPU uses its own I2C interface
static const I2CCC26XX_I2CPinCfg mpuI2cPinConfig = {
    .pinSDA = Board_I2C0_SDA1,
    .pinSCL = Board_I2C0_SCL1};

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
    PIN_TERMINATE};

/*
 *  Button functions
 */

bool eatButtonPressed = FALSE;
enum Music music_selection = SILENT;
void button0_Fxn(PIN_Handle handle, PIN_Id pinId)
{
   music_selection++;
   nowPlaying(music_selection, messageBuffer);
   //testMessage(messageBuffer);
   if (music_selection == END)
      music_selection = SILENT;
}

void button1_Fxn(PIN_Handle handle, PIN_Id pinId)
{
   //eat(1, messageBuffer);
   //eatButtonPressed = TRUE;
   toggleLed(ledHandle, Board_LED0);
   sendSensorDataToBackend = !sendSensorDataToBackend;
}

void buzzerTaskFxn()
{
   while (1)
   {
        
      if (inDistress)
      {
         makeSound(buzzerHandle, ALERT);
         inDistress = 0;
      }
      if (eatButtonPressed)
      {
         makeSound(buzzerHandle, EAT);
         eatButtonPressed = FALSE;
      }
      
      makeSound(buzzerHandle, music_selection);
      
       Task_sleep(SECOND/5);
   }
}

/**
 * react to
 */
void Beep(char* warningMsg)
{
    inDistress = 1;

    char msg[BUFFERSIZE] = "MSG2:";
    strcat(msg, warningMsg);
    write_to_messageBuffer(messageBuffer, msg);
}

static void checkMessage(UART_Handle handle, void *rxBuf, size_t len){
    char* token = strtok(rxBuf, ",");
    char msg[BUFFERSIZE];
    if (atoi(token) == GROUP_ID_NUM){
        token = strtok(NULL, ":");
        if (strcmp(token, "BEEP") == 0){
            token = strtok(NULL,",");
            strcpy(msg,token);
            Beep(msg);
        }
    }
    UART_read(handle, rxBuf, BUFFERSIZE);
    Task_sleep(SECOND/5);
}

void uartTaskFxn()
{
   UART_Handle uartHandle;
   UART_Params uartParams;

   UART_Params_init(&uartParams);
   uartParams.writeDataMode = UART_DATA_TEXT;
   uartParams.readDataMode = UART_DATA_TEXT;
   uartParams.readEcho = UART_ECHO_OFF;
   uartParams.readMode = UART_MODE_CALLBACK;
   uartParams.readCallback = &checkMessage;
   uartParams.baudRate = 9600;            // nopeus 9600baud
   uartParams.dataLength = UART_LEN_8;    // 8
   uartParams.parityType = UART_PAR_NONE; // n
   uartParams.stopBits = UART_STOP_ONE;   // 1

   uartHandle = UART_open(Board_UART0, &uartParams);
   if (uartHandle == NULL)
   {
      System_abort("Error opening the UART");
   }
   UART_read(uartHandle, receiveBuffer, BUFFERSIZE);
   while (1)
   {
      // TODO: korvaa tilakoneella
      // Jos viestibufferissa on dataa, lähetetään se ja nollataan bufferi
      if (messageBuffer[0] != '\0')
      {
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
Void sensorTaskFxn()
{
   I2C_Handle i2c_mpu9250, i2c_opt3001, i2c_bmp280;
   I2C_Params i2cParams_mpu9250, i2cParams_opt3001, i2cParams_bmp280;

   sensorSetup(&i2c_mpu9250, &i2c_opt3001, &i2c_bmp280, &i2cParams_mpu9250, &i2cParams_opt3001, &i2cParams_bmp280);
   write_to_messageBuffer(messageBuffer, "session:start");


   while (1)
   {
      time = Clock_getTicks() / Clock_tickPeriod;
      time = time / 1000;

      /*
       * Get data from sensors
       */
      i2c_mpu9250 = I2C_open(Board_I2C_TMP, &i2cParams_mpu9250);
      if (i2c_mpu9250 == NULL)
         System_abort("Error Initializing mpu9250 I2C\n");
      // Task_sleep(SECOND / 10);

      mpu9250_get_data(&i2c_mpu9250, &ax, &ay, &az, &gx, &gy, &gz);

      I2C_close(i2c_mpu9250);
      // Task_sleep(SECOND / 10);

      // OPT3001
      i2c_opt3001 = I2C_open(Board_I2C_TMP, &i2cParams_opt3001);
      if (i2c_opt3001 == NULL)
         System_abort("Error Initializing opt3001 I2C\n");
      // Task_sleep(SECOND);

      light = opt3001_get_data(&i2c_opt3001);

      I2C_close(i2c_opt3001);
      // Task_sleep(SECOND / 10);

      // BMP280
      i2c_bmp280 = I2C_open(Board_I2C_TMP, &i2cParams_bmp280);
      if (i2c_bmp280 == NULL)
         System_abort("Error Initializing mpu9250 I2C\n");
      // Task_sleep(SECOND / 10);

      bmp280_get_data(&i2c_bmp280, &press, &temp);

      I2C_close(i2c_bmp280);

      /*
         */
      clean_mpu9250_data(&ax, &ay, &az, &gx, &gy, &gz);
      // TODO: toteuta clean_other_data (testaa ja keksi sopivat rajat)

      write_sensor_readings_to_sensorDataArray(sensorDataArray, sensorArrayHEAD, time, ax, ay, az, gx, gy, gz, temp, press, light);

      detectPets();
      if(detect_Exercise(mpu9250DeltasArray)){
        // exercise(5, messageBuffer);
         //makeSound(buzzerHandle, DOOM);
      }
      calculate_mpu9250_deltas(sensorDataArray, mpu9250DeltasArray);

      if (sendSensorDataToBackend == TRUE)
      {
         if (timesSenttoBackend == 0)
            write_to_messageBuffer(messageBuffer, "session:start");
         
         write_sensor_readings_to_messageBuffer(messageBuffer, time, ax, ay, az, gx, gy, gz, temp, press, light);
         
         if (timesSenttoBackend == 20){
            write_to_messageBuffer(messageBuffer, "session:end");
            sendSensorDataToBackend = FALSE;
            timesSenttoBackend = -1;
         }
         timesSenttoBackend++;
      }

      // Used to turn the sensor data array into a ring buffer
      sensorArrayHEAD++;
      if (sensorArrayHEAD == SENSOR_DATA_ROWS)
          sensorArrayHEAD = 0;

      Task_sleep(SECOND / 10);
   }
}
int petAmount = 0;
void detectPets(){
    float lightAmount = sensorDataArray[sensorArrayHEAD][LIGHT];
    if (lightAmount > 0){
        if (lightAmount > 10|| lightAmount < 20){
            petAmount++;
            if (petAmount == 3){
                petAmount = 0;
                pet(5, messageBuffer);
                makeSound(buzzerHandle,ONEUP);
            }
        }
    }
}

int main(void)
{

   Task_Handle sensorTaskHandle;
   Task_Params sensorTaskParams;
   Task_Handle uartTaskHandle;
   Task_Params uartTaskParams;
   Task_Handle buzzerTaskHandle;
   Task_Params buzzerTaskParams;

   Board_initGeneral();
   initialize_handles();

   // Sensor Task
   Task_Params_init(&sensorTaskParams);
   sensorTaskParams.stackSize = STACKSIZE;
   sensorTaskParams.stack = &sensorTaskStack;
   sensorTaskParams.priority = 2;
   sensorTaskHandle = Task_create(sensorTaskFxn, &sensorTaskParams, NULL);
   if (sensorTaskHandle == NULL)
   {
      System_abort("Task create failed!");
   }

   // UART Task
   Task_Params_init(&uartTaskParams);
   uartTaskParams.stackSize = STACKSIZE;
   uartTaskParams.stack = &uartTaskStack;
   uartTaskParams.priority = 2;
   uartTaskHandle = Task_create(uartTaskFxn, &uartTaskParams, NULL);
   if (uartTaskHandle == NULL)
   {
      System_abort("Task create failed!");
   }

   // Buzzer Task
   Task_Params_init(&buzzerTaskParams);
   buzzerTaskParams.stackSize = STACKSIZE;
   buzzerTaskParams.stack = &buzzerTaskStack;
   buzzerTaskHandle = Task_create((Task_FuncPtr)buzzerTaskFxn, &buzzerTaskParams, NULL);
   if (buzzerTaskHandle == NULL)
   {
      System_abort("Buzzer task create failed!");
   }

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
void initialize_handles()
{
   mpuPinHandle = PIN_open(&mpuPinState, mpuPowerPinConfig);
   if (!mpuPinHandle)
   {
      System_abort("Error initializing MPU power pin\n");
   }

   // Ledi käyttöön ohjelmassa
   ledHandle = PIN_open(&ledState, ledConfig);
   if (!ledHandle)
   {
      System_abort("Error initializing LED pin\n");
   }

   // Painonappi 0 käyttöön ohjelmassa
   button0_Handle = PIN_open(&button0_State, button0_Config);
   if (!button0_Handle)
   {
      System_abort("Error initializing button0 pin\n");
   }
   // Painonappi 1 käyttöön ohjelmassa
   button1_Handle = PIN_open(&button1_State, button1_Config);
   if (!button1_Handle)
   {
      System_abort("Error initializing button1 pin\n");
   }

   // Painonapille 0 keskeytyksen käsittellijä
   if (PIN_registerIntCb(button0_Handle, &button0_Fxn) != 0)
   {
      System_abort("Error registering button callback function");
   }

   // Painonapille 1 keskeytyksen käsittellijä
   if (PIN_registerIntCb(button1_Handle, &button1_Fxn) != 0)
   {
      System_abort("Error registering button callback function");
   }

   // Buzzer
   buzzerHandle = PIN_open(&buzzerState, buzzerConfig);
   if (buzzerHandle == NULL)
   {
      System_abort("Buzzer pin open failed!");
   }
}

void initialize_task(Task_Handle *handle, Task_Params *params, void(*taskFxn), char *stack, uint8_t priority)
{
   Task_Params_init(params);
   (*params).stackSize = STACKSIZE;
   (*params).stack = &stack;

   (*handle) = Task_create(taskFxn, &params, NULL);

   if (handle == NULL)
   {
      System_abort("Task create failed!");
   }
   if (priority != 0)
      (*params).priority = priority;
}

void sensorSetup(I2C_Handle *i2c_mpu9250, I2C_Handle *i2c_opt3001, I2C_Handle *i2c_bmp280,
                 I2C_Params *i2cParams_mpu9250, I2C_Params *i2cParams_opt3001, I2C_Params *i2cParams_bmp280)
{
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

   // Alustetaan OPT3001
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
