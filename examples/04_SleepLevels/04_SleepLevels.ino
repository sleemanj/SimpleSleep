/** This example shows you the different levels of sleep, use your multimeter 
 *   to measure the current of each one.
 *
 * Notice how after doing a Serial print we have to ensure long enough time has
 *   gone by for that to have been sent before we go into a deep or light sleep
 *   in this case we just add a 15mS idle, if the BAUD rate was faster it could 
 *   be less, if there were things happening between the print and he sleep
 *   then it could also be less or not required at all.  In short, remember 
 *   Serial (hardware at least) works asynchronously.
 *   
 */
 
#include <SimpleSleep.h>

SimpleSleep Sleep;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  Serial.println(F("No Sleep"));
  delay(5000);
  
  Serial.println(F("Deep Sleep"));
  Sleep.idleFor(15); // We should wait a little before sleeping deeply or lightly so Serial finishes up.
  Sleep.deeplyFor(5000);
  
  Serial.println(F("Light Sleep"));
  Sleep.idleFor(15); // We should wait a little before sleeping deeply or lightly so Serial finishes up.
  Sleep.lightlyFor(5000);
  
  Serial.println(F("Idle Sleep"));
  Sleep.idleFor(5000);
}