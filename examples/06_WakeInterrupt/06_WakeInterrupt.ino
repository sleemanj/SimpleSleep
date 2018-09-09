/** This example shows you how to wake from an external interrupt rather than waiting for a given length of time.
 * 
 * Connect a button betwen the interrupt pin and ground, press said button to trigger an interrupt and wake.
 * 
 */

#include <SimpleSleep.h>
SimpleSleep Sleep;

const uint8_t ledPin = 13;
const uint8_t interruptPin = 2;

void setup()
{
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), interruptHandler, LOW);
}

void interruptHandler()
{
  // In this example, do nothing we will just wakeup and continue from where-ever
  // we went to sleep, of course maybe you might want to do something else on
  // the interrupt also, up to you.
}

void loop()
{
  // Blink once then sleep until an external interrupt happens
  digitalWrite(ledPin, HIGH); delay(50); digitalWrite(ledPin, LOW);

  Sleep.deeply();
}
