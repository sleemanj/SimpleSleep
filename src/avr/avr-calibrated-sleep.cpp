/** This file contains implementation of calibration which are common amongst AVR chips.
 *  
 *  Keep ifdef to a minimum, use variant implementation files if there is any substantial difference.
 */

#if defined (__AVR__)
  
  #include "../SimpleSleep.h"
  
  #ifdef NO_MILLIS
    // Some of the Arduino cores, particularly @sleemanj's ATTinyCore fork,
    //  allow disabling millis, in which case we can not do any 
    //
    // @TODO Maybe there is some way to do it using a delay() (which will
    //   still work without millis()), like, start the WDT then start a delay()
    //   if the WDT triggers before the delay() time then the WDT needed to be longer
    //   if it hasn't finished when the delay has it needed to be shorter...
    //
    SimpleSleep_Cal SimpleSleep::getCalibration()
    {
      return 1;
    }
      
    void SimpleSleep::deeplyFor(uint32_t sleepMs, SimpleSleep_Cal calData)
    {
      (void)(calData); // Silence unused warning
      deeplyFor(sleepMs);
    }

    void SimpleSleep::lightlyFor(uint32_t sleepMs, SimpleSleep_Cal calData)
    {
      (void)(calData); // Silence unused warning
      lightlyFor(sleepMs);
    }

    void SimpleSleep::idleFor(uint32_t sleepMs, SimpleSleep_Cal calData)
    {
      (void)(calData); // Silence unused warning
      idleFor(sleepMs);
    }

  #elif SS_USE_INT_CAL == 1

    __attribute__((weak)) SimpleSleep_Cal SimpleSleep::getCalibration()
    {
      SimpleSleep_Cal calData;
      
      uint32_t m = millis();
      idleFor(15);
      m = millis() - m;
      calData.adjust15MS = 15 - m;
      
      m = millis();
      idleFor(250);
      m=millis() - m;
      calData.adjust250MS = 250 - m;
      
      return calData;
    }

    __attribute__((weak)) void SimpleSleep::deeplyFor(uint32_t sleepMs, SimpleSleep_Cal calData)
    {
      sleepDeeply(sleepMs + ((sleepMs/250)*calData.adjust250MS) + (((sleepMs - ((sleepMs/250)*250))/15)*calData.adjust15MS));
    }

    __attribute__((weak)) void SimpleSleep::lightlyFor(uint32_t sleepMs, SimpleSleep_Cal calData)
    {
      lightlyFor(sleepMs + ((sleepMs/250)*calData.adjust250MS) + ((sleepMs - ((sleepMs/250)*250))/15)*calData.adjust15MS);
    }

    __attribute__((weak)) void SimpleSleep::idleFor(uint32_t sleepMs, SimpleSleep_Cal calData)
    {
      idleFor(sleepMs + ((sleepMs/250)*calData.adjust250MS) + ((sleepMs - ((sleepMs/250)*250))/15)*calData.adjust15MS);
    }

  #else

    __attribute__((weak)) SimpleSleep_Cal SimpleSleep::getCalibration()
    {
      uint32_t m = millis();
      idleFor(15);
      m = millis() - m;
      return (float)15 / (float)m;
    }

    __attribute__((weak)) void SimpleSleep::deeplyFor(uint32_t sleepMs, SimpleSleep_Cal calData)
    {
      deeplyFor(sleepMs * calData);
    }

    __attribute__((weak)) void SimpleSleep::lightlyFor(uint32_t sleepMs, SimpleSleep_Cal calData)
    {
      lightlyFor(sleepMs * calData);
    }

    __attribute__((weak)) void SimpleSleep::idleFor(uint32_t sleepMs, SimpleSleep_Cal calData)
    {
      idleFor(sleepMs * calData);
    }
  #endif
#endif