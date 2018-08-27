#ifndef SS_ATMegax8_h
#define SS_ATMegax8_h

  #if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__) || defined (__AVR_ATmega328PB__) \
  || defined (__AVR_ATmega168__)  \
  || defined (__AVR_ATmega168A__) || defined (__AVR_ATmega168P__) \
  || defined (__AVR_ATmega168PA__)|| defined (__AVR_ATmega168PB__) \
  || defined (__AVR_ATmega48__) || defined (__AVR_ATmega48A__) \
  || defined (__AVR_ATmega48P__) || defined (__AVR_ATmega48PA__) || defined (__AVR_ATmega48PB__) \
  || defined (__AVR_ATmega88__) || defined (__AVR_ATmega88A__) \
  || defined (__AVR_ATmega88P__) || defined (__AVR_ATmega88PA__) || defined (__AVR_ATmega88PB__) \
  || defined (__AVR_ATmega8__) || defined (__AVR_ATmega8A__)

    #define SS_SUPPORTED_CHIP
    #define SS_ATMegax8

  #endif
  
#endif