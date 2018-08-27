#ifndef SimpleSleep_h
#define SimpleSleep_h

#include <Arduino.h>

#include "avr/avr.h"

/** Simple Sleep class for Arduino.
 * 
 * SimpleSleep Sleep;
 * 
 * Sleep.forever(); // Deep dreamless slumber, bod off, adc off, interrupts off
 * Sleep.deeply();  // Wakes up on external interrupts, Power Save mode or Power Down mode, bod off, adc off
 * Sleep.lightly(); // Wakes up on externl interrupts, some internal interrupts, timers generally off, adc off
 * Sleep.idle();    // Wakes up on most anything, timers still run, adc is on
 * 
 * Sleep.deeplyFor(arbitraryMs);
 * Sleep.lightlyFor(arbitraryMs);
 * Sleep.idleFor(arbitraryMs);
 * 
 */

  class SimpleSleep
  {
    public:
      
     /** Sleep forever in a deep and dreamless slumber.  
      * 
      *  Everything that can be shut down is shut down, not even interrupts will wake you.  
      *  The only way to recover from forever() is to reset. 
      * 
      *  This is a one-way operation, minimum power consumption.
      *
      */
      
      inline void forever()                   { sleepForever(); }
      
      /** Sleep deeply, allow external interrupts where possible (LEVEL only usually), bod off, adc off, timers generally off
       * 
       * For AVR, typically implemented as Power Down
       */
    
      inline void deeply()                    { sleepDeeply(0);       }
      inline void deeplyFor(uint32_t sleepMs) { sleepDeeply(sleepMs); }
      
      
      /** Sleep lightly, allow many interrupts, adc off, timers generally off
       * 
       *  For AVR, typically either implemented as Extended Standby or ADC Noise Reduction with the ADC **OFF**.
       */
      
      inline void lightly()                    { sleepLightly(0); }
      inline void lightlyFor(uint32_t sleepMs) { sleepLightly(sleepMs); }
      
      /** Wait patiently, most anything can wake you including Serial, timers etc. 
       *
       * For AVR, typically implemented as Idle
       */
      
      inline void idle()                        { sleepIdle(0);       }
      inline void idleFor(uint32_t sleepMs)     { sleepIdle(sleepMs); }
      
      /** For more accurate sleep times, you can generate calibration data and pass
       *  it into the deeplyFor, lightlyFor and idleFor.
       * 
       * Getting the calibration will take some time, more than 355 mS, during which
       * time the system is in idle() mode.
       * 
       * You probably want to get fresh calibration data somewhat regularly, especially
       * if you experience temperature changes or voltage changes.
       */
      
      SimpleSleep_Cal getCalibration();
      void deeplyFor(uint32_t sleepMs, SimpleSleep_Cal calibrationData);
      void lightlyFor(uint32_t sleepMs, SimpleSleep_Cal calibrationData);
      void idleFor(uint32_t sleepMs, SimpleSleep_Cal calibrationData);
      
      
    protected:
    
      void sleepForever();
      void sleepDeeply(uint32_t sleepMs);
      void sleepLightly(uint32_t sleepMs);
      void sleepIdle(uint32_t sleepMs);
      
  };

#endif