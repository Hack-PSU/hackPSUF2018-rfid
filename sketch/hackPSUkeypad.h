#pragma once



#include <Arduino.h>

namespace hackPSU{
  #define READ_PERIOD 4    // setup period before each keypad read
  
  class Keypad{
    private:
      int pin, sig, clk;
      char keys[4][4] = {  {'1', '2', '3', 'A'},
                           {'4', '5', '6', 'B'},
                           {'7', '8', '9', 'C'},
                           {'*', '0', '#', 'D'} };
      void pulse(bool data, int space = 2);
      void (*clear)();
      void (*putChar)(char);

      void clearSR() { shiftOut(sig, clk, MSBFIRST, B00000000); };
    public:

      /**
       * INPUTS: 
       *  KPD_SRC - analog pin which is connected to resistor tree
       *  KPD_CLK - pin connected clock of shift register
       *  KPD_SIG - pin connected to data signal of shift register
       *  
       *  clear - method to clear the LCD after a clear was detected
       *  putChar - method to display a character on the LCD (and/or serial port) as keys are pressed
       *  
       */
      Keypad(int KPD_SRC, int KPD_CLK, int KPD_SIG, void (*clear)(void), void (*putChar)(char));

      /**
       * INPUT:
       *  none
       *  
       * OUTPUT: 
       *  either key pressed or 'x' if no key pressed
       */
      char readKeypad();

      /**
       * INPUT:
       *  none
       *  
       * OUTPUT:
       *  key once it is released or 'u' after a timeout was reached
       */
      char getUniqueKey(unsigned long timeout);

      /**
       * INPUTS:
       *  stable - number of consecutive reads to ensure stable input
       *  timeout - number of milliseconds function can wait for a stable input
       *  
       * OUTPUT:
       *  key pressed or 't' on timeout
       */
      char getKeyPress(int stable, unsigned long timeout);

      /**
       * INPUT:
       *  
       * OUTPUT:
       *  Number of characters that make up the pin
       */
      String getPin(int maxLen, char clr, char submit, int timeout);
  };
  
}

