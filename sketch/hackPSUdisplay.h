#pragma once

#include <Wire.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "hackPSUconfig.h"

namespace hackPSU {

  typedef enum {PROD, DEV, HEADLESS} Mode_e;
  class Display{
    private:
      Mode_e mode;
      LiquidCrystal_I2C* lcd;
      String data[2];
      int row;

    public:
      Display(Mode_e mode);
      void print(String msg);
      void print(char msg);
      void print(String msg, int row);
      void clear();
      void clear(int row);
      void backspace(int num);
  };

}
