/** This example shows you how to use idle mode to sleep while maintaining serial communications.
 * 
 *  We ask the user a question, then go into idle mode until they answer saving a little power
 *  (not as much as lightly or deeply sleeping, but still less power waste than just delay()).
 * 
 *  We also make use of deepSleep mode during doing what the user asked us to do (that is
 *  blink an LED some number of times).
 */

#include <SimpleSleep.h>
SimpleSleep Sleep; 

const uint8_t ledPin = 13;

void setup()
{
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  // Ask the user a question...
  Serial.println(F("How many times to flash 0-99: "));

  // Wait using low-power idle...
  while(!Serial.available())
  {
    Sleep.idle();
  }

  // Read their answer...
  char buf[4];
  Serial.readBytesUntil('\n', buf, sizeof(buf)-1 );

  // And do a thing...
  for( int8_t x = atoi(buf); x > 0; x--)
  {      
    digitalWrite(ledPin, HIGH);
    Sleep.deeplyFor(1000); // (we don't need to listen to serial here)

    digitalWrite(ledPin, LOW);
    Sleep.deeplyFor(1000); // (we don't need to listen to serial here)
  }
}