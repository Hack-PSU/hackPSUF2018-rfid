#pragma once

#include <Wire.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

//#include "hackPSUconfig.h"

namespace hackPSU {

  typedef enum {PROD, DEV, HEADLESS} Mode_e;
  typedef enum {CHECK_C, UP_C, DOWN_C, CLEAR_C, BACK_C, LOCK_C, SCROLL_C, NONE_C} Custom_char;
  struct Menu{
    char key;
    Custom_char symbol;
  };
  class Display{
    private:
      Mode_e mode;
      LiquidCrystal_I2C* lcd;
      String data[2];
      Menu menu[4];
      int row;
      bool scrolled;
      
      void print(Custom_char symbol);
      void print(Menu control);

    public:
      Display(Mode_e mode);
      ~Display();

      /**
       * Description:
       *    Prints msg starting at the current location of the cursor
       */
      void print(String msg);

      /**
       * Description:
       *    Prints the character at the current location of the cursor
       */
      void print(char msg);

      /**
       * Description:
       *    Clears the specified row then prints msg on that row leaving the cursor at the end of the string.
       *    If msg is currently displayed on the specified row, it will not be displayed.
       */
      void print(String msg, int row);


      /**
       * Description:
       *    Prints a menu
       */

      void print(char key1, Custom_char symbol1, char key2, Custom_char symbol2, char key3, Custom_char symbol3, char key4, Custom_char symbol4);
      
      /**
       * Description:
       *    Scrolls contents of the LCD
       */ 
      void scroll();

       /**
       * Description:
       *    Clears all contents of the LCD and returns the cursor to (0, 0)
       */
      void clear();

      /**
       * Description:
       *    Clears the requested row then sets the cursor to the beginning of that row
       */
      void clear(int row);

      /**
       * Description:
       *    Clears the last <num> characters and moved the cursor back to the new end of the string
       */
      void backspace(int num);
  };
}
