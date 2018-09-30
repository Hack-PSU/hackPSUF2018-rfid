#include "hackPSUdisplay.h"

namespace hackPSU {
  Display::Display(Mode_e mode) : mode(mode){
    row = 0;
    data[0] = "";
    data[1] = "";
    
    if(mode == PROD || mode == DEV){
      const uint8_t I2C_ADDRESSES[] = {0x27, 0x3f};
      Wire.begin(/*SDA, SCL*/);
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
      Serial.begin(BAUD_RATE);
      Serial.println("Started serial communication");
    }
      
  }

  Display::~Display(){
    delete lcd;
  }

  void Display::print(char msg){
    data[row] += String(msg);
    if(mode == PROD || mode == DEV){
      if(data[row].length() > 16){
        clear(row);
        lcd->setCursor(0, row);
        lcd->print(data[row].substring(data[row].length()-16));
      } else {
        lcd->print(msg);
      }
    }
    if(mode == DEV || mode == HEADLESS) {
      Serial.println(data[row]);
    }
  }
  
  void Display::print(const char *msg){
    data[row] += String(msg);
    if(mode == PROD || mode == DEV){
      if(data[row].length() > 16){
        clear(row);
        lcd->setCursor(0, row);
        lcd->print(data[row].substring(0,16));
      } else {
        lcd->print(msg);
      }
    }
    if(mode == DEV || mode == HEADLESS) {
      Serial.println(String(msg) + " - " + row);
    }
  }

  void Display::print(const char *msg, int row){
    if(strncmp(msg,data[row].c_str(), 16)){
      Serial.println(msg);
      Serial.println(data[row]);
      if(mode == PROD || mode == DEV){
        clear(row);
        lcd->setCursor(0, row);
        lcd->print(msg);
      }
      if(mode == DEV || mode == HEADLESS) {
        Serial.println(msg);
      }
      data[row] = String(msg);
      this->row = row;
    }
  }

  void Display::clear(){
    data[0] = "";
    data[1] = "";
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
    data[row] = "";
  }

  void Display::backspace(int num){
    int pos = data[row].length() - num;
    lcd->setCursor(pos, row);
    for(int i = 0; i < num; i++){
      lcd->print(' ');
    }
    lcd->setCursor(pos, row);
    data[row] = data[row].substring(0, pos);
  }
};
