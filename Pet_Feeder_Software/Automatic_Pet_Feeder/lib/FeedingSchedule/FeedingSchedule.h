#include <Arduino.h>
#include <EEPROM.h>

#define MAX_BT_BUFFER 13
#define EEPROM_SIZE 18 //6 alarms (hour , minute , nr of portions)
void FeedingSchedule_Pet(void);

extern SemaphoreHandle_t mutexData;
extern int receivedBTdataBuffer[MAX_BT_BUFFER];

struct BTtime_fs{
  int hour;
  int minute;
  int second;
};

enum scheduleNumer
{
  schdlNr0 = 0,
  schdlNr1 = 1,
  schdlNr2 = 2,
  schdlNr3 = 3,
  schdlNr4 = 4,
  schdlNr5 = 5
};