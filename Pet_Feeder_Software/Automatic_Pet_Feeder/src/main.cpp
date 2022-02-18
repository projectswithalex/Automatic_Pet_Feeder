#include <Arduino.h>
#include <Wire.h>
#include "header.h"
#include "BluetoothSerial.h"
#include "MotorControl.h"
#include "Display.h"
#include "FeedingSchedule.h"

/**************************** Global Variables *******************/

int receivedBTdataBuffer[MAX_BT_BUFFER] = {0xFF}; // Buffer to Store the received Bluetooth Data.
BTtime petFeeder_time = {0, 0, 0};
/**************************** Global Variables *******************/

/**************************** Objects ****************************/

static BluetoothSerial ESP32_Pet_Feeder; // BluetoothSerial Object

/**************************** Objects ****************************/

/**************************** Functions **************************/

extern void DisplayTask_Pet(void);
extern void MotorControlTask_Pet(void);
extern void FeedingSchedule_Pet(void);
void setTime();

/**************************** Functions ****************************/

/**************************** Task Handle ****************************/
static TaskHandle_t bltask_handle = NULL;
static TaskHandle_t motorControlTask_handle = NULL;
static TaskHandle_t feedingScheduleTask_handle = NULL;
static TaskHandle_t displayTask_handle = NULL;
SemaphoreHandle_t mutexData;

/**************************** Task Handle ****************************/

/**************************** Task Definition ****************************/

/*
 * Description: Bluetooth/Scheduler Task
 * Function : Receive Bluetooth mesages and activate corespoding Task based on the key received
 */
void BL_Task(void *parameters)
{
  int l_btBufferCounter = 0;

  while (1)
  {
    if (xSemaphoreTake(mutexData, 0) == pdTRUE)
    {
      while (ESP32_Pet_Feeder.available())
      {
        receivedBTdataBuffer[l_btBufferCounter] = ESP32_Pet_Feeder.read();
        Serial.print("BT Data :");
        Serial.println(receivedBTdataBuffer[l_btBufferCounter]);
        l_btBufferCounter++;
      }
      l_btBufferCounter = 0;
      xSemaphoreGive(mutexData);
    }

    switch (receivedBTdataBuffer[BT_CONTROL_COMMAND])
    {
    case dateAndTime:
      setTime();

      break;

    case feedOnePortion:
      Serial.println("1");
      xTaskNotify(motorControlTask_handle, feedOnePortion, eSetBits);
      Serial.println("2");
      receivedBTdataBuffer[BT_CONTROL_COMMAND] = (int)eraseCommand;
      Serial.println("3");
      break;

    case feedXportions:
      xTaskNotify(motorControlTask_handle, feedXportions, eSetBits);
      receivedBTdataBuffer[BT_CONTROL_COMMAND] = (int)eraseCommand;
      break;

    case setSchedule:
      xTaskNotifyGive(feedingScheduleTask_handle);
      receivedBTdataBuffer[BT_CONTROL_COMMAND] = (int)eraseCommand;
      break;

    default:
      break;
    }
    l_btBufferCounter = 0;

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

/*
 * Description: Motor Control Task
 */

void motorControl_Task(void *parameters)
{
  MotorControlTask_Pet();
}

/*
 * Description: Feeding Schedule Task
 */

void feedingSchedule_Task(void *parameters)
{
  FeedingSchedule_Pet();
}

/*
 * Description: Display Task
 */

void display_Task(void *parameters)
{

  DisplayTask_Pet();
}

/**************************** Task Definition ****************************/

/*
 * Description: Setup function to run once at "boot-up"
 */

void setup()
{

  mutexData = xSemaphoreCreateMutex();

  // Bluetooth/Scheduler Task Creation
  xTaskCreate(
      BL_Task,          // function Name
      "Bluetooth Task", // Task name
      1024,             // Stack Size
      NULL,             // Task parameters
      2,                // Task Priority
      &bltask_handle    // Task Handle
  );

  // Bluetooth/Scheduler Task Creation
  xTaskCreate(
      motorControl_Task,       // Task Name
      "Motor Control Task",    // Task name
      1024,                    // Stack Size
      NULL,                    // Task parameters
      1,                       // Task Priority
      &motorControlTask_handle // Task Handle
  );
  // Bluetooth/Scheduler Task Creation
  xTaskCreate(
      feedingSchedule_Task,       // Task Name
      "Feeding Schedule Task",    // Task name
      1024,                       // Stack Size
      NULL,                       // Task parameters
      1,                          // Task Priority
      &feedingScheduleTask_handle // Task Handle
  );
  // Bluetooth/Scheduler Task Creation
  xTaskCreate(
      display_Task,       // Task Name
      "Display Task",     // Task name
      1024,               // Stack Size
      NULL,               // Task parameters
      1,                  // Task Priority
      &displayTask_handle // Task Handle
  );

  // Start Serial Monitor (For Debug purpose)
  Serial.begin(19200);
  delay(2000);

  // Start BL Communication
  ESP32_Pet_Feeder.begin("ESP32_Control");
  delay(2000);
}
void loop()
{
  vTaskDelete(NULL);
}

void setTime(void)
{
  petFeeder_time.hour = receivedBTdataBuffer[BTTIME_HOUR_IDX];
  petFeeder_time.minute = receivedBTdataBuffer[BTTIME_MINUTE_IDX];
  petFeeder_time.second = receivedBTdataBuffer[BTTIME_SECOND_IDX];
}