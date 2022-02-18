#include <Arduino.h>
#include "BasicStepperDriver.h"
#define BT_CONTROL_COMMAND 0

enum commands_motor
{
  dateAndTime = 0,
  feedOnePortion = 1,
  feedXportions = 2,
  setSchedule = 3,
  stopSchedule = 4,
  deleteSchedule = 5,
  eraseCommand=0xFF
};
// Connections to A4988 Motor Driver
#define DIR_PIN  33  // Direction pin
#define STEP_PIN  32 // Step pin

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define STEPS_PER_REV 200
#define RPM 120

// Since microstepping is set externally, make sure this matches the selected mode
// If it doesn't, the motor will move at a different RPM than chosen
// 1=full step, 2=half step etc.
#define MICROSTEPS 1
#define MAX_BT_BUFFER 13
//Uncomment line to use enable/disable functionality
//#define SLEEP 13
//extern enum commands;
extern SemaphoreHandle_t mutexData;
extern int receivedBTdataBuffer[MAX_BT_BUFFER];
void MotorControlTask_Pet(void);