/** This example simply puts the chip to sleep forever, immediately.  
 *
 *  You can use this to see the minimum current draw of your setup..
 */
 
#include <SimpleSleep.h>

SimpleSleep Sleep;

void setup()
{

}

void loop()
{
  Sleep.forever();
}