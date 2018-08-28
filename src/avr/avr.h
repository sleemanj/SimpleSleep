/**  Support functions and macros which are common across AVR microcontrollers.
 * 
 *   @file 
 */

#ifndef SS_AVR_h
#define SS_AVR_h
#if defined (__AVR__)

#include <Arduino.h>

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>

/** Include all the variant .h files */

#include "ATMegaX8.h"
#include "ATTinyX5.h"
#include "ATTinyX4.h"
#include "ATTiny13.h"

#ifndef SS_SUPPORTED_CHIP
  #error "SimpleSleep does not support this microcontroller."
#endif

/** Determine if to use the integer type of calibration, or a floating.
 * 
 * Undefine the below to use floating, which may be more accurate, but heavier weight.
 */

#ifndef SS_USE_INT_CAL
  #define SS_USE_INT_CAL 1
#endif

#ifdef NO_MILLIS

    /* If millis is not available, then we can not really do calibration,
     *  the calibration will always just return 1
     */
    
   typedef uint8_t SimpleSleep_Cal;
   
#elif SS_USE_INT_CAL == 1

/** The WDT on AVR generally has two ranges, 
 *     15ms->120ms and 250ms->8000ms (some, 2000ms)
 *  if we idle for 15ms and for 250ms and record the offset against what
 *  millis() reported then we can add/subtract appropriately in the 
 *  calibrated versions of deeply/lightly/idle
 * 
 *  Note that we are using a signed byte here for each calibration,
 *  this *should* be ok, the WDT would have to be like 50% out to
 *  cause a problem there and that is, I hope, unlikely.
 */

struct SimpleSleep_Cal
{
  int8_t adjust15MS   = 0; // Increase or decrease the sleepMs by this many mS every 15mS
  int8_t adjust250MS  = 0; // Increase or decrease the sleepMs by this many mS every 250mS
};

#else

/** The float calibration simply records a multiplier to adjust sleepMs by.
 */

typedef float SimpleSleep_Cal;
#endif

/** Most chips have a watchdog interrupt, but some do not. */
#ifndef WDT_HAS_INTERRUPT
  #if !defined(WDIE) && !defined(WDTIE)
    #define WDT_HAS_INTERRUPT 0
  #else
    #define WDT_HAS_INTERRUPT 1
  #endif
#endif

/** Determine the WDT period (avr/wdt.h) which is necessary to sleep for next
 *   in order to get closer tot he sleepMs, also deduct that many mS from sleepMs
 *  
 * This is defined here in the header so that it is inlined since it's most likely
 *   to only be used in one place by each variant (and there will only be one 
 *   variant active), so the overhead of calling it as an extern, with LTO off 
 *   at least, is substantial.
 */

inline uint16_t wdt_period_for(uint32_t *sleepMs)
{
  #ifdef WDP3
    //  8000, 4000, 2000, 1000, 500, 250, and then 120, 60, 30, 15
    uint16_t period = 8000;
    int8_t x = 9;
  #else
    //  2000, 1000, 500, 250, and then 120, 60, 30, 15
    uint16_t period = 2000;
    int8_t x = 7;
  #endif
  
  while(x >= 1)
  {
    if(*sleepMs >= period)
    {
      *sleepMs -= period;
      return x;
    }
    
    x--; 
    period/=2; 
    
    // There are two sequences, 8000->250 and 120->15 so that you don't 
    // end up fractional,so once we hit 125, drop down to the 120->15 seequence
    if(period == 125) 
    {
      period = 120;
    }
  }
  
  // The sleep time is less than 30mS (x == 1),  if it's greater than 15mS, 
  //  spin-wait until it's 15mS then allow the WDT to do the rest, if it's less 
  //  than 15mS then 15mS it is, that is the minimum we can sleep for and we
  //  MUST sleep.
  
  if(*sleepMs > 15)
  {
    delay(*sleepMs - 15);
  }
  
  *sleepMs = 0;
  return WDTO_15MS;
}

/** Macro for declaring backup variables for all the Power Reduction Register
 *   values, across (hopefully) all AVR variants using one macro call.
 * 
 *  Additional keywords to be used int he declaration (eg, static) can be 
 *   provided.
 * 
 *  Usage:
 *    power_declare_all(static);
 *    power_save_all();
 *    ...do stuff to the power reduction registers...
 *    power_restore_all();
 * 
 * 
 */

#define power_declare_all(...) \
  power_declare_prr(__VA_ARGS__); \
  power_declare_prr0(__VA_ARGS__);\
  power_declare_prr1(__VA_ARGS__);\
  power_declare_prr2(__VA_ARGS__);\
  power_declare_prpa(__VA_ARGS__);\
  power_declare_prpb(__VA_ARGS__);\
  power_declare_prpc(__VA_ARGS__);\
  power_declare_prpd(__VA_ARGS__);\
  power_declare_prpe(__VA_ARGS__);\
  power_declare_prpf(__VA_ARGS__);\
  power_declare_prgen(__VA_ARGS__);

  
/** Macro for saving all the Power Reduction Register values, across 
 *   (hopefully) all AVR variants using one macro call.
 * 
 *  Usage:
 *    power_declare_all();
 *    power_save_all();
 *    ...do stuff to the power reduction registers...
 *    power_restore_all();
 */

#define power_save_all() \
  power_save_prr(); \
  power_save_prr0();\
  power_save_prr1();\
  power_save_prr2();\
  power_save_prpa();\
  power_save_prpb();\
  power_save_prpc();\
  power_save_prpd();\
  power_save_prpe();\
  power_save_prpf();\
  power_save_prgen();

  
/** Macro for restoring all the Power Reduction Register values, across 
 *   (hopefully) all AVR variants using one macro call.
 * 
 *  Usage:
 *    power_declare_all();
 *    power_save_all();
 *    ...do stuff to the power reduction registers...
 *    power_restore_all();
 */

#define power_restore_all() \
  power_restore_prr(); \
  power_restore_prr0();\
  power_restore_prr1();\
  power_restore_prr2();\
  power_restore_prpa();\
  power_restore_prpb();\
  power_restore_prpc();\
  power_restore_prpd();\
  power_restore_prpe();\
  power_restore_prpf();\
  power_restore_prgen();

// The below register names are extraced from
// cat ./hardware/tools/avr/avr/include/avr/power.h | grep "define powe" | grep -Po "[0-9A-Z_]+ &" | sort | uniq
  
#ifdef PRR
  #define power_declare_prr(...)  __VA_ARGS__ uint8_t oldPRR;
  #define power_save_prr()      oldPRR = PRR;
  #define power_restore_prr()   PRR = oldPRR;
#else
  #define power_declare_prr(...)
  #define power_save_prr()      
  #define power_restore_prr()   
#endif

#ifdef PRR0
  #define power_declare_prr0(...)  __VA_ARGS__ uint8_t oldPRR0;
  #define power_save_prr0()      oldPRR0 = PRR0;
  #define power_restore_prr0()   PRR = oldPRR0;
#else
  #define power_declare_prr0(...)
  #define power_save_prr0()      
  #define power_restore_prr0()   
#endif

#ifdef PRR1
  #define power_declare_prr1(...)  __VA_ARGS__ uint8_t oldPRR1;
  #define power_save_prr1()      oldPRR1 = PRR1;
  #define power_restore_prr1()   PRR = oldPRR1;
#else
  #define power_declare_prr1(...)
  #define power_save_prr1()      
  #define power_restore_prr1()   
#endif

#ifdef PRR2
  #define power_declare_prr2(...)  __VA_ARGS__ uint8_t oldPRR2;
  #define power_save_prr2()      oldPRR2 = PRR2;
  #define power_restore_prr2()   PRR = oldPRR2;
#else
  #define power_declare_prr2(...)
  #define power_save_prr2()      
  #define power_restore_prr2()   
#endif

#ifdef PR_PRPF
  #define power_declare_prpf(...)  __VA_ARGS__ uint8_t oldPRPF;
  #define power_save_prpf()      oldPR_PRPF = PR_PRPF;
  #define power_restore_prpf()   PR_PRPF = oldPR_PRPF;
#else
  #define power_declare_prpf(...)
  #define power_save_prpf()     
  #define power_restore_prpf()  
#endif

#ifdef PR_PRPE
  #define power_declare_prpe(...)  __VA_ARGS__ uint8_t oldPRPE;
  #define power_save_prpe()      oldPR_PRPE = PR_PRPE;
  #define power_restore_prpe()   PR_PRPE = oldPR_PRPE;
#else
  #define power_declare_prpe(...)
  #define power_save_prpe()      
  #define power_restore_prpe()   
#endif

#ifdef PR_PRPD
  #define power_declare_prpd(...)  __VA_ARGS__ uint8_t oldPRPD;
  #define power_save_prpd()      oldPR_PRPD = PR_PRPD;
  #define power_restore_prpd()   PR_PRPD = oldPR_PRPD;
#else
  #define power_declare_prpd(...)
  #define power_save_prpd()      
  #define power_restore_prpd()   
#endif

#ifdef PR_PRPC
  #define power_declare_prpc(...)  __VA_ARGS__ uint8_t oldPRPC;
  #define power_save_prpc()      oldPR_PRPC = PR_PRPC;
  #define power_restore_prpc()   PR_PRPC = oldPR_PRPC;
#else
  #define power_declare_prpc(...)
  #define power_save_prpc()      
  #define power_restore_prpc()   
#endif

#ifdef PR_PRPB
  #define power_declare_prpb(...)  __VA_ARGS__ uint8_t oldPRPB;
  #define power_save_prpb()      oldPR_PRPB = PR_PRPB;
  #define power_restore_prpb()   PR_PRPB = oldPR_PRPB;
#else
  #define power_declare_prpb(...)
  #define power_save_prpb()      
  #define power_restore_prpb()   
#endif

#ifdef PR_PRPA
  #define power_declare_prpa(...)  __VA_ARGS__ uint8_t oldPRPA;
  #define power_save_prpa()      oldPR_PRPA = PR_PRPA;
  #define power_restore_prpa()   PR_PRPA = oldPR_PRPA;
#else
  #define power_declare_prpa(...)
  #define power_save_prpa()      
  #define power_restore_prpa()   
#endif

#ifdef PR_PRGEN
  #define power_declare_prgen(...)  __VA_ARGS__ uint8_t oldPRGEN;
  #define power_save_prgen()     oldPR_PRGEN = PR_PRGEN;
  #define power_restore_prgen()   PR_PRGEN = oldPR_PRGEN;
#else
  #define power_declare_prgen(...)
  #define power_save_prgen()     
  #define power_restore_prgen()   
#endif

#if defined(PRR) || defined(PRR0) || defined(PRR1) || defined(PRR2) || \
    defined( PR_PRPA ) || defined( PR_PRPB ) || defined( PR_PRPC ) || defined( PR_PRPD ) || defined( PR_PRPE ) || defined( PR_PRPF ) || \
    defined(PRGEN) 
    
    #define power_has_power() 1
#else
    #define power_has_power() 0
#endif
#endif
#endif