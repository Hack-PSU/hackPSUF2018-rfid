#pragma once

#include <LiquidCrystal_I2C.h>

namespace hackPSU {

  typedef enum {PROD, DEV, HEADLESS} Mode_e;
  class Display{
    private:
      static Mode_e mode;
      // PROD     - print to LCD only
      // DEV      - print to both LCD and Serial
      // HEADLESS - print only to Serial
      static LiquidCrystal_I2C* lcd;
      
    public:
      Display(Mode_e mode);
      ~Display();
      static void print(char character);
      static void print(String msg);
      static void print(String msg, int row);
      static void clear(void);
      static void clear(int row);
  };

}
