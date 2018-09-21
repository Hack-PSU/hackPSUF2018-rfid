#include "hackPSUdisplay.h"

namespace hackPSU {
  Display::Display(Mode_e mode){
    this->mode = mode;
    if(mode == PROD || mode == DEV){
        const uint8_t I2C_ADDRESSES[] = {0x27, 0x3f};
        Wire.begin(/*SRA, SCL*/);
        for(uint8_t i : I2C_ADDRESSES){
          Wire.beginTransmission(i);
          if(Wire.endTransmission() == 0){
            lcd = new LiquidCrystal_I2C(i, 16, 2);
      //      Serial.print("I2C LCD found at 0x" + i < 16 ? "0":"");
      //      Serial.println(i, HEX);
            break;
          }
        }
        lcd->init();
        lcd->clear();
        lcd->backlight();
        lcd->setCursor(0,0);
      }
      if(mode == DEV || mode == HEADLESS) {
        Serial.begin(9600);
      }
      
  }

  void Display::print(char msg){
    if(mode == PROD || mode == DEV){
      lcd->print(msg);
    }
    if(mode == DEV || mode == HEADLESS) {
      Serial.println(msg);
    }
  }
  
  void Display::print(String msg){
    if(mode == PROD || mode == DEV){
      lcd->print(msg);
    }
    if(mode == DEV || mode == HEADLESS) {
      Serial.println(msg);
    }
  }

  void Display::print(String msg, int row){
    if(mode == PROD || mode == DEV){
      clear(row);
      lcd->setCursor(0, row);
      lcd->print(msg);
    }
    if(mode == DEV || mode == HEADLESS) {
      Serial.println(msg);
    }
  }

  void Display::clear(){
    if(mode != HEADLESS) {
      lcd->clear();
    }
  }

  void Display::clear(int row){
    if(mode != HEADLESS) {
      lcd->setCursor(0, row);
      lcd->print("                ");
      lcd->setCursor(0, row);
    }
  }
};
