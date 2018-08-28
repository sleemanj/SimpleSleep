/** This file contains implementation of methods in SimpleSleep class which are common amongst AVR chips.
 * 
 *  Keep ifdef to a minimum, use variant implementation files if there is any substantial difference.
 */

#if defined (__AVR__)

#include "../SimpleSleep.h"

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

  
  static void untimed_sleep(uint8_t mode, uint8_t bod, uint8_t interrupts);

  __attribute__((weak)) void SimpleSleep::sleepForever() 
  {
    // ADC OFF
    //  (setting ADCSRA = 0 here would probably be fine,
    //   but it makes no difference to the optimiser)
    ADCSRA &= ~(1 << ADEN);
    
    // All power reduction (probably doesn't make a difference in PWR_DOWN anyway)
    power_all_disable();
    
    // sleep with bod off, interrupts off
    untimed_sleep(SLEEP_MODE_PWR_DOWN, false, false);
  }
  
  __attribute__((weak)) void SimpleSleep::sleepDeeply()
  {
    // ADC OFF
    uint8_t oldADCSRA = ADCSRA;
    ADCSRA &= ~(1 << ADEN);
    
    power_declare_all();
    power_save_all();
    power_all_disable();
    
    // sleep with bod off, interrupts on
    untimed_sleep(SLEEP_MODE_PWR_DOWN, false, true);
    
    power_restore_all();
    ADCSRA = oldADCSRA;
  }
  
  __attribute__((weak)) void SimpleSleep::sleepLightly()
  {
    // ADC OFF
    uint8_t oldADCSRA = ADCSRA;
    ADCSRA &= ~(1 << ADEN);
    
    // sleep with bod off, interrupts on    
    #ifdef SLEEP_MODE_EXT_STANDBY
      untimed_sleep(SLEEP_MODE_EXT_STANDBY, false, true);
    #else
      untimed_sleep(SLEEP_MODE_ADC, false, true);
    #endif
  
    
    ADCSRA = oldADCSRA;
  }
  
  __attribute__((weak)) void SimpleSleep::sleepIdle()
  { 
    // sleep with bod on, interrupts on
    untimed_sleep(SLEEP_MODE_IDLE, true, true);
  }

  static void untimed_sleep(uint8_t mode, uint8_t bod, uint8_t interrupts)
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
  
#endif
