#pragma once

#include <LiquidCrystal_I2C.h>

namespace hackPSU {

  typedef enum {PROD, DEV, HEADLESS} Mode_e;
  class Display{
    private:
      Mode_e mode;
      // PROD     - print to LCD only
      // DEV      - print to both LCD and Serial
      // HEADLESS - print only to Serial
      LiquidCrystal_I2C* lcd;
      
    public:
      Display(Mode_e mode);
      ~Display();
      void print(char character);
      void print(String msg);
      void print(String msg, int row);
      void clear(void);
      void clear(int row);
  };

}
