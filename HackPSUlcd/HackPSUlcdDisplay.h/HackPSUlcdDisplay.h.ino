#pragma once

#include <SPI.h>
#include <LiquidCrystal_I2C.h>

namespace hackPSU {

  class LcdDisplay{
    public:
      void printMsg(String msg);
      void printLine(String msg);
      void clearScreen();
      void clearRow(int row);
  };

}
