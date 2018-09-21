#include "hackPSUdisplay.h"

namespace hackPSU {
  Display::Display(State_e state){
    this->state = state;
    if(state == PROD || state == DEV){
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
      if(state == DEV || state == HEADLESS) {
        Serial.begin(9600);
      }
      
  }

  void Display::print(char msg){
    if(state == PROD || state == DEV){
      lcd->print(msg);
    }
    if(state == DEV || state == HEADLESS) {
      Serial.println(msg);
    }
  }
  
  void Display::print(String msg){
    if(state == PROD || state == DEV){
      lcd->print(msg);
    }
    if(state == DEV || state == HEADLESS) {
      Serial.println(msg);
    }
  }

  void Display::print(String msg, int row){
    if(state == PROD || state == DEV){
      clear(row);
      lcd->setCursor(0, row);
      lcd->print(msg);
    }
    if(state == DEV || state == HEADLESS) {
      Serial.println(msg);
    }
  }

  void Display::clear(){
    if(state != HEADLESS) {
      lcd->clear();
    }
  }

  void Display::clear(int row){
    if(state != HEADLESS) {
      lcd->setCursor(0, row);
      lcd->print("                ");
      lcd->setCursor(0, row);
    }
  }
};
