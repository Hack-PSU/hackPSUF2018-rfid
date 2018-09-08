#pragma once

namespace hackPSU{
  class keypad{
    private:
      int pin;
      char keys[4][4] = {  {'1', '2', '3', 'A'},
                           {'4', '5', '6', 'B'},
                           {'7', '8', '9', 'C'},
                           {'*', '0', '#', 'D'} };
      void pulse(int space);
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
      keypad(int KPD_SRC, int KPD_CLK, int KPD_SIG, void (*clear)(), void (*putChar)(char));

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
      char getUniqueKey();
      void getPin(char* buffer, int length, char clr, char submit);
  };
  
}

