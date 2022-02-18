#include "MotorControl.h"
#define CLEAR_TASK_NOTIFICATION 0xFFFFFFFF
// Stepper Motor Object
static BasicStepperDriver nema17step(STEPS_PER_REV, DIR_PIN, STEP_PIN);

void MotorControlTask_Pet(void)
{
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  nema17step.begin(RPM, MICROSTEPS);

  uint32_t notificationValue = 0xFF;

  while (1)
  {

    if (xTaskNotifyWait(CLEAR_TASK_NOTIFICATION, 0, &notificationValue, 100/portTICK_PERIOD_MS) == pdTRUE)
    {
      Serial.println("4");
      if (xSemaphoreTake(mutexData, 0) == pdTRUE)
      {
        
          Serial.println("5");
        switch ((commands_motor)notificationValue)
        {
        case feedOnePortion:
          Serial.println("6");
          nema17step.rotate(180);
          break;

        case feedXportions:
          Serial.println("7");
          nema17step.rotate(180 * receivedBTdataBuffer[BT_CONTROL_COMMAND + 1]);
          Serial.print("Value feed portions");
          Serial.println(receivedBTdataBuffer[BT_CONTROL_COMMAND + 1]);

          break;

        default:
          break;
        }
        xSemaphoreGive(mutexData);
      }   
    }

    // vTaskDelay(100/portTICK_PERIOD_MS);
    // vTaskSuspend(NULL);
  }
}
