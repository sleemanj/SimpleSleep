/** This file contains implementation of methods in SimpleSleep class which are common amongst AVR chips.
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

SimpleSleep_Cal SimpleSleep::getCalibration()
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

void SimpleSleep::deeplyFor(uint32_t sleepMs, SimpleSleep_Cal calData)
{
  sleepDeeply(sleepMs + ((sleepMs/250)*calData.adjust250MS) + (((sleepMs - ((sleepMs/250)*250))/15)*calData.adjust15MS));
}

void SimpleSleep::lightlyFor(uint32_t sleepMs, SimpleSleep_Cal calData)
{
  lightlyFor(sleepMs + ((sleepMs/250)*calData.adjust250MS) + ((sleepMs - ((sleepMs/250)*250))/15)*calData.adjust15MS);
}

void SimpleSleep::idleFor(uint32_t sleepMs, SimpleSleep_Cal calData)
{
  idleFor(sleepMs + ((sleepMs/250)*calData.adjust250MS) + ((sleepMs - ((sleepMs/250)*250))/15)*calData.adjust15MS);
}

#else

SimpleSleep_Cal SimpleSleep::getCalibration()
{
  uint32_t m = millis();
  idleFor(15);
  m = millis() - m;
  return (float)15 / (float)m;
}

void SimpleSleep::deeplyFor(uint32_t sleepMs, SimpleSleep_Cal calData)
{
  deeplyFor(sleepMs * calData);
}

void SimpleSleep::lightlyFor(uint32_t sleepMs, SimpleSleep_Cal calData)
{
  lightlyFor(sleepMs * calData);
}

void SimpleSleep::idleFor(uint32_t sleepMs, SimpleSleep_Cal calData)
{
  idleFor(sleepMs * calData);
}
#endif

/** SS_SUPPORTED_CHIP will be defined by each variant's .h file */
#ifndef SS_SUPPORTED_CHIP
  #error "SimpleSleep does not support this microcontroller."
#endif

 /** Standard AVR implementation of the SimpleSleep methods.
  * 
  *  It happens that the method for putting AVR's to sleep are often very very common
  *  between chips, so rather than do a lot of copy-paste we will define these
  *  as standard methods.
  * 
  *  Do not do "#ifdef __AVR_*******__" here, if you think you need something
  *  special for some variant, over-ride the methods in the variant's file.
  * 
  *  These are defined as weak so that each variant can over-ride the method 
  *  if necessary.
  */

  inline static void timed_sleep(uint32_t sleepMs, uint8_t mode, uint8_t bod, uint8_t interrupts);
  inline static void untimed_sleep(uint8_t mode, uint8_t bod, uint8_t interrupts);

  __attribute__((weak)) void SimpleSleep::sleepForever() 
  {
    // ADC OFF
    ADCSRA &= ~(1 << ADEN);
    
    // All power reduction (probably doesn't make a difference in PWR_DOWN anyway)
    power_all_disable();
    
    // sleep with bod off, interrupts off
    untimed_sleep(SLEEP_MODE_PWR_DOWN, false, false);
  }
  
  __attribute__((weak)) void SimpleSleep::sleepDeeply(uint32_t sleepMs) 
  {
    // ADC OFF
    uint8_t oldADCSRA = ADCSRA;
    ADCSRA &= ~(1 << ADEN);
    
    power_declare_all();
    power_save_all();
    power_all_disable();
    
    // sleep with bod off, interrupts on
    if(sleepMs)
    {
      // For a timed sleep we may need millis() in order
      // to make up sleep to a multiple of 15mS (min sleep period)
      // so we will need to leave timer0 powered up (obviously
      //  during the actual power-down it won't be counting
      //  we only need it to count between the power-down periods
      //  of which a timed_sleep might be made up of more than one).
      #if power_has_power()
        power_timer0_enable();
      #endif
      timed_sleep(sleepMs, SLEEP_MODE_PWR_DOWN, false, true);
    }
    else
    {
      untimed_sleep(SLEEP_MODE_PWR_DOWN, false, true);
    }
    
    power_restore_all();
    ADCSRA = oldADCSRA;
  }
    
  __attribute__((weak)) void SimpleSleep::sleepLightly(uint32_t sleepMs)
  {
    // ADC OFF
    uint8_t oldADCSRA = ADCSRA;
    ADCSRA &= ~(1 << ADEN);
    
    // sleep with bod off, interrupts on
    if(sleepMs)
    {
      #ifdef SLEEP_MODE_EXT_STANDBY
        timed_sleep(sleepMs, SLEEP_MODE_EXT_STANDBY, false, true);
      #else
        timed_sleep(sleepMs, SLEEP_MODE_ADC, false, true);
      #endif
    }
    else
    {
      #ifdef SLEEP_MODE_EXT_STANDBY
        untimed_sleep(SLEEP_MODE_EXT_STANDBY, false, true);
      #else
        untimed_sleep(SLEEP_MODE_ADC, false, true);
      #endif
    }
    
    ADCSRA = oldADCSRA;
  }
  
  __attribute__((weak)) void SimpleSleep::sleepIdle(uint32_t sleepMs)
  { 
    if(!sleepMs)
    {
      // sleep with bod on, interrupts on
      untimed_sleep(SLEEP_MODE_IDLE, true, true);
    }
    else
    {
      timed_sleep(sleepMs, SLEEP_MODE_IDLE, true, true);
    }
  }
  
  inline static void untimed_sleep(uint8_t mode, uint8_t bod, uint8_t interrupts)
  {
    set_sleep_mode(mode);
    cli();        
    sleep_enable();
    
    #ifdef sleep_bod_disable
    if(!bod)
    {
      sleep_bod_disable();
    }
    #else
    (void)(bod); // Silence warning
    #endif

    // Caution, with interrupts disabled the only way you are likely
    // to wake up is with a reset
    if(interrupts)
    {
      sei();
    }

    sleep_cpu();      
    sleep_disable();
    sei();
  }


#if  WDT_HAS_INTERRUPT == 1
#warning "USING WDT"
  volatile uint8_t wdt_triggered = 1;
  
  ISR (WDT_vect) 
  {
    wdt_disable();  
    wdt_triggered = 1;
  }
  
  inline static void timed_sleep(uint32_t sleepMs, uint8_t mode, uint8_t bod, uint8_t interrupts)
  {   
    do
    {
      // If we are not waiting on the WDT, and there is time still to sleep, setup the WDT (again)
      if (wdt_triggered && sleepMs)
      {
        wdt_triggered = 0;
        wdt_enable(wdt_period_for(&sleepMs));
        WDTCSR |= (1 << WDIE);  
      }
    
      set_sleep_mode(mode);
      cli();        
      sleep_enable();
      #ifdef sleep_bod_disable
        if(!bod)
        {
          sleep_bod_disable();
        }
      #else
        (void)(bod); // Silence warning
      #endif
      
      // Caution, with interrupts disabled the only way you are likely
      // to wake up is with a reset
      if(interrupts)
      {
        sei();
      }
      
      sleep_cpu();      
      sleep_disable();
      sei();
    } while(!wdt_triggered || sleepMs > 0);
  }
  
#else

  /** This implements timed sleep without WDT, instead we force into IDLE mode and just
   *   spin-wait until the time is up.  millis() must be available.
   *
   */
  
  inline static void timed_sleep(uint32_t sleepMs, uint8_t mode, uint8_t bod, uint8_t interrupts)
  {
    mode = SLEEP_MODE_IDLE;   
    uint32_t startSleep = millis();
    
    do
    {
      // If we are not waiting on the WDT, and there is time still to sleep, setup the WDT (again)
      if((millis() - startSleep)>=sleepMs)
      {
        return;
      }
    
      
      set_sleep_mode(mode);
      cli();        
      sleep_enable();
      #ifdef sleep_bod_disable
        if(!bod)
        {
          sleep_bod_disable();
        }
      #else
        (void)(bod); // Silence warning
      #endif
      
      // Caution, with interrupts disabled the only way you are likely
      // to wake up is with a reset
      if(interrupts)
      {
        sei();
      }
      
      sleep_cpu();      
      sleep_disable();
      sei();
    } while(sleepMs > 0);
  
  }
#endif
  
  
#endif
