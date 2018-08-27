#ifndef SS_ATtiny13_h
#define SS_ATtiny13_h

  #if defined (__AVR_ATtiny13__) || defined (__AVR_ATtiny13A__) 
    #define SS_SUPPORTED_CHIP
    #define SS_ATTiny13
  
    // Rename these for Tiny13
    //  (Tiny13 name on the right, "standard" (MegaX8) name on the left)
    #define WDTCSR WDTCR
    #define WDIE   WDTIE
    
    // While we do have a WDT interrupt, it uses quite a lot of 
    #define WDT_HAS_INTERRUPT 1
    #if ! defined( WDT_HAS_INTERRUPT ) && ! defined( NO_MILLIS )
      #define WDT_HAS_INTERRUPT 0
    #endif
    
  #endif    
#endif