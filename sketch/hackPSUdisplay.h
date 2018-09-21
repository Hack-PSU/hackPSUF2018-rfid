#pragma once

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

namespace hackPSU {

  typedef enum {PROD, DEV, HEADLESS} State_e;

  class Display{
    private:
      State_e state;
      LiquidCrystal_I2C* lcd;
    public:
      Display(State_e state);
      void print(String msg);
      void print(char msg);
      void print(String msg, int row);
      void clear();
      void clear(int row);

  };

}
