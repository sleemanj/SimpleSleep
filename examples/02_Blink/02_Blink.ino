/** This example works just like normal Blink except that it goes to sleep instead of just `delay()`.
 *
 */
 
#include <SimpleSleep.h>

SimpleSleep Sleep;

const uint8_t ledPin = 13;

void setup()
{
  pinMode(13, OUTPUT);
}

void loop()
{
  digitalWrite(ledPin, HIGH);
  Sleep.deeplyFor(1000);
  
  digitalWrite(ledPin, LOW);
  Sleep.deeplyFor(1000);
}