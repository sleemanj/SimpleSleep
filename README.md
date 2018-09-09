# SimpleSleep

An Arduino library for simple means of putting your Arduino into sleep modes in order to save power when you are not doing much, supports a variety of common microcontrollers used with Arduino...

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents** 

- [Supported Chips](#supported-chips)
  - [ATMega Series](#atmega-series)
  - [ATTiny Series](#attiny-series)
- [Download, Install and Example](#download-install-and-example)
- [Code Examples](#code-examples)
  - [Deep and Dreamless Slumber (Forever Sleep)](#deep-and-dreamless-slumber-forever-sleep)
  - [Very Low Power Blink](#very-low-power-blink)
  - [Somewhat Low Power Blink](#somewhat-low-power-blink)
  - [Slightly Low Power Blink but (Hardware) Serial Still Works and millis() is still accurate](#slightly-low-power-blink-but-hardware-serial-still-works-and-millis-is-still-accurate)
  - [Calibrated Low Power Blink](#calibrated-low-power-blink)
  - [Sleep deeply, but would wake up if there was an interrupt.](#sleep-deeply-but-would-wake-up-if-there-was-an-interrupt)
- [Full Class Reference](#full-class-reference)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## Supported Chips

### ATMega Series

  * ATMega328 Series
  * ATMega168 Series
  * ATMega48/88
  * ATMega8
  
### ATTiny Series

  * ATTiny25/45/85
  * ATTiny24/44/84
  * ATTiny13 ( recommended to use [this core](https://github.com/sleemanj/optiboot/blob/master/dists/README.md#attiny13) )


## Download, Install and Example

* Download: http://sparks.gogo.co.nz/SimpleSleep.zip
* Open the Arduino IDE (this library was tested in 1.8.5, older versions may work)
* Select the menu item Sketch > Include Library > Add Zip Library
* Choose to install the SimpleSleep.zip file you downloaded
* Now you can choose File > Examples > SimpleSleep > HelloWorld

Upload the example to your Arduino and stand back in awe of the results. 

## Code Examples

### Deep and Dreamless Slumber (Forever Sleep)

Sleeping forever is the simplest type of sleep and uses the least power, but only a RESET (or power-cycle of course) will wake you up.  This can be very useful for "hit the button to do a thing" type devices, "hit the button play a sound", "hit the button light a light"... you just stick your button between RESET and Ground then in your code sleep forever when you have done the thing you want to do every time you hit the button...

    #include <SimpleSleep.h>
    SimpleSleep Sleep; 
    
    void setup()
    {
      
    }

    void loop()
    {
      /* Do something interesting here */
      
      /* After you are done, sleep forever. */
      Sleep.forever(); 
    }
    
Note that sleep forever does not alter your pin states, a HIGH pin will still be HIGH, a LOW pin will still be LOW, pullups will still be on if so configured.  So you should also disable those (set to INPUT is often best for minimising power consumption, but it depends on your devices).

### Very Low Power Blink

    #include <SimpleSleep.h>
    SimpleSleep Sleep; 
    
    const uint8_t ledPin = 13
    
    void setup()
    {
      pinMode(ledPin, OUTPUT);
    }

    void loop()
    {
      digitalWrite(ledPin, HIGH);
      Sleep.deeplyFor(1000);
      
      digitalWrite(ledPin, LOW);
      Sleep.deeplyFor(1000);
    }

        
### Somewhat Low Power Blink

The difference between sleeping deeply and sleeping lightly is chip dependant.  In terms of AVR, light sleeping is generally implemented as Extended Stand By or ADC Noise Reduction (with ADC off) modes, this uses more power than deeply but less than idle.

    #include <SimpleSleep.h>
    SimpleSleep Sleep; 
    
    const uint8_t ledPin = 13
    
    void setup()
    {
      pinMode(ledPin, OUTPUT);
    }

    void loop()
    {
      digitalWrite(ledPin, HIGH);
      Sleep.lightlyFor(1000);
      
      digitalWrite(ledPin, LOW);
      Sleep.lightlyFor(1000);
    }
    
### Slightly Low Power Blink but (Hardware) Serial Still Works and millis() is still accurate

Using lightly/deeply generally is going to stop your serial communications and your millis count will not count during sleeping.  The simplest way around this (there are others) is to use "idle" mode, which basically leaves everything running as much as possible, including serial and timers.


    #include <SimpleSleep.h>
    SimpleSleep Sleep; 
    
    const uint8_t ledPin = 13
    
    void setup()
    {
      pinMode(ledPin, OUTPUT);
      Serial.begin(9600);
    }

    void loop()
    {
      // Ask the user a question...
      Serial.println(F("How many times to flash 0-99: "));
      
      // Wait using low-power idle...
      while(!Serial.available())
      {
        Sleep.idle();
      }
      
      // Read their answer...
      char buf[4];
      Serial.readBytesUntil('\n', buf, sizeof(buf)-1 );
      
      // And do a thing...
      for( int8_t x = atoi(buf); x >= 0; x--)
      {      
        digitalWrite(ledPin, HIGH);
        Sleep.deeplyFor(1000); // (we don't need to listen to serial here)
        
        digitalWrite(ledPin, LOW);
        Sleep.deeplyFor(1000); // (we don't need to listen to serial here)
      }
    }

### Calibrated Low Power Blink

Doing a calibration, which takes up to a few hundred milliseconds, can make for more 
accurate timing...


    #include <SimpleSleep.h>
    SimpleSleep     Sleep; 
    SimpleSleep_Cal SleepCal;
    
    const uint8_t ledPin = 13
    
    void setup()
    {
      SleepCal = Sleep.getCalibration();
      pinMode(ledPin, OUTPUT);
    }

    void loop()
    {
      digitalWrite(ledPin, HIGH);
      Sleep.deeplyFor(1000, SleepCal);
      
      digitalWrite(ledPin, LOW);
      Sleep.deeplyFor(1000, SleepCal);
    }
    

### Sleep deeply, but would wake up if there was an interrupt.

Only LEVEL type interrupts generally work and they must be longer than usual
so if you connect a button between the interruptPin and ground, hold it down


    #include <SimpleSleep.h>
    SimpleSleep Sleep;
    
    const uint8_t ledPin = 13
    const uint8_t interruptPin = 2;
    
    void setup()
    {
      pinMode(ledPin, OUTPUT);
      pinMode(interruptPin, INPUT_PULLUP);
      attachInterrupt(digitalPinToInterrupt(interruptPin), interruptHandler, LOW);
    }
    
    void interruptHandler()
    {
      // In this example, do nothing we will just wakeup and continue from where-ever
      // we went to sleep.
    }
    
    void loop()
    {
      // Blink once then sleep until an external interrupt happens
      digitalWrite(ledPin, HIGH); delay(50); digitalWrite(ledPin, LOW);
      
      Sleep.deeply();
    }
    
Sleeping lightly ( ` Sleep.lightly() ` ) can also be used (equates to Extended Stand-By where available)


## Full Class Reference

I recommend to just look at the examples which show you how to use all the features, but if you want the nitty-gritty then here is the [full class reference](https://rawgit.com/sleemanj/SimpleSleep/e5c029a/docs/html/class_simple_sleep.html)

Thanks to the super kind folks of [RawGit](https://rawgit.com/) and [MaxCDN](http://www.maxcdn.com/)
