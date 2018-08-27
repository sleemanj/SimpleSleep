#ifndef SS_ATtinyX5_h
#define SS_ATtinyX5_h

  #if defined (__AVR_ATtiny85__) || defined (__AVR_ATtiny45__) || defined (__AVR_ATtiny25__)
  
    #define SS_SUPPORTED_CHIP
    #define SS_ATTinyX5
  
    // T85 uses WDTCR instead of WDTCSR
    #define WDTCSR WDTCR
    
  #endif    
#endif