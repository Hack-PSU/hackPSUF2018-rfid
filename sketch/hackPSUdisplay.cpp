#include "hackPSUdisplay.h"

namespace hackPSU{
  
  Display::Display(Mode_e mode/*, uint16_t SDA, uint16_t SCK*/){
    // Create lcd object based on which address the LCD is associated with
    this->mode = mode;
    if(mode == PROD || mode == DEV){
      const byte I2C_ADDRESSES[] = {0x27, 0x3f};
      Wire.begin(/*SRA, SCL*/);
      for(byte i : I2C_ADDRESSES){
        Wire.beginTransmission(i);
        if(Wire.endTransmission() == 0){
          lcd = new LiquidCrystal_I2C(i, 16, 2);
    //      Serial.print("I2C LCD found at 0x" + i < 16 ? "0":"");
    //      Serial.println(i, HEX);
          break;
        }
      }
    }
  }

  Display::~Display(){
    delete lcd;
  }

  void Display::print(char character){
    lcd->print(character);
  }

  void Display::print(String msg) {
    if(mode == PROD || mode == DEV){
      // Print to the LCD
    }
    if( mode == DEV || mode == HEADLESS){
      // Print to Serial port
    }
    lcd->setCursor(0, 0);
    lcd->print(msg);
  }
  
  void Display::print(String msg, int row) {
    if(mode == PROD || mode == DEV){
      // Print to the LCD
    }
    if( mode == DEV || mode == HEADLESS){
      // Print to Serial port (don't care about row number)
    }
    lcd->setCursor(row, 0);
    // Might want to clear before writing in the case that the previous message is longer than the current message
    lcd->print(msg); 
  }

  void Display::clear(){
    if(mode == DEV || mode == HEADLESS){
      // clear 
    }
    // This will work but we could also use the clear(int) call and call with `0' and `1'
    lcd->clear();
  }
  
  void Display::clear(int row){
    if(mode == DEV || mode == HEADLESS){
      // clear 
    }
    lcd->setCursor(row, 0);
    lcd->print("                ");//16 spaces
  }
}

