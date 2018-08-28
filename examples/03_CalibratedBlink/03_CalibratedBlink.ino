/** This example works just like normal Blink except that it goes to sleep instead of just `delay()`.
 *
 *  In this version we use a calibration step to get a more accurate sleep time.
 */
 
#include <SimpleSleep.h>

SimpleSleep Sleep;
SimpleSleep_Cal SleepCal;

const uint8_t ledPin = 13;

void setup()
{
  pinMode(13, OUTPUT);
  
  SleepCal = Sleep.getCalibration();  
}

void loop()
{
  digitalWrite(ledPin, HIGH);
  Sleep.deeplyFor(1000, SleepCal);
  
  digitalWrite(ledPin, LOW);
  Sleep.deeplyFor(1000, SleepCal);
}