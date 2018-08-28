/** This file contains implementation of timed sleeping which are common amongst AVR chips.
 * 
 *  SimpleSleep methods in this file are typically declared as weak so that variants may 
 *  define their own.
 *  
 *  Keep ifdef to a minimum, use variant implementation files if there is any substantial difference.
 */

#if defined (__AVR__)
  
  #include "../SimpleSleep.h"

  static void timed_sleep(uint32_t sleepMs, uint8_t mode, uint8_t bod, uint8_t interrupts);

  __attribute__((weak)) void SimpleSleep::sleepDeeply(uint32_t sleepMs) 
  {
    // ADC OFF
    uint8_t oldADCSRA = ADCSRA;
    ADCSRA &= ~(1 << ADEN);
    
    power_declare_all();
    power_save_all();
    power_all_disable();
    
    
    // For a timed sleep we may need millis() in order
    // to make up sleep to a multiple of 15mS (min sleep period)
    // so we will need to leave timer0 powered up (obviously
    //  during the actual power-down it won't be counting
    //  we only need it to count between the power-down periods
    //  of which a timed_sleep might be made up of more than one).
    #if power_has_power()
      power_timer0_enable();
    #endif
      
    // sleep with bod off, interrupts on
    timed_sleep(sleepMs, SLEEP_MODE_PWR_DOWN, false, true);
  
    
    power_restore_all();
    ADCSRA = oldADCSRA;
  }
  
  __attribute__((weak)) void SimpleSleep::sleepLightly(uint32_t sleepMs)
  {
    // ADC OFF
    uint8_t oldADCSRA = ADCSRA;
    ADCSRA &= ~(1 << ADEN);
    
    // sleep with bod off, interrupts on
    #ifdef SLEEP_MODE_EXT_STANDBY
      timed_sleep(sleepMs, SLEEP_MODE_EXT_STANDBY, false, true);
    #else
      timed_sleep(sleepMs, SLEEP_MODE_ADC, false, true);
    #endif

    ADCSRA = oldADCSRA;
  }
  
  __attribute__((weak)) void SimpleSleep::sleepIdle(uint32_t sleepMs)
  { 
    timed_sleep(sleepMs, SLEEP_MODE_IDLE, true, true);
  }
  
  #if  WDT_HAS_INTERRUPT == 1
    volatile uint8_t wdt_triggered = 1;
    
    ISR (WDT_vect) 
    {
      wdt_disable();  
      wdt_triggered = 1;
    }
    
    static void timed_sleep(uint32_t sleepMs, uint8_t mode, uint8_t bod, uint8_t interrupts)
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
    
    void timed_sleep(uint32_t sleepMs, uint8_t mode, uint8_t bod, uint8_t interrupts)
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