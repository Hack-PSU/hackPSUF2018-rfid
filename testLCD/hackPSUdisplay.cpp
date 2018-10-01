#include "hackPSUdisplay.h"

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

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

    //creating custom characters:
    uint8_t check[8] = {0x0,0x1,0x3,0x16,0x1c,0x8,0x0};
    uint8_t clear[8] = {0x0,0x1b,0xe,0x4,0xe,0x1b,0x0};
    uint8_t upArrow[8] = {0x0, 0x4, 0xe, 0x1f, 0x4, 0x4, 0x4, 0x0};
    uint8_t downArrow[8] = {0x0, 0x4, 0x4, 0x4, 0x1f, 0xe, 0x4, 0x0};
    uint8_t backArrow[8] = {0x0, 0x4, 0x8, 0x1f, 0x9, 0x5, 0x0, 0x0};
    uint8_t lock[8] = {0xe, 0xa, 0x1f, 0x11, 0x1b, 0x1b, 0x1f, 0x0};
    uint8_t scroll[8] = {0x2, 0x1f, 0x2, 0x0, 0x8, 0x1f, 0x8, 0x0};
    
    lcd->createChar(CHECK, check);
    lcd->createChar(UP, upArrow);
    lcd->createChar(DOWN, downArrow);
    lcd->createChar(CLEAR, clear);
    lcd->createChar(BACK, backArrow);
    lcd->createChar(LOCK, lock);
    lcd->createChar(SCROLL, scroll);
    
    if(mode == DEV || mode == HEADLESS) {
      Serial.begin(9600);
      Serial.println("Started serial communication");
    }
      
  }

  Display::~Display(){
    delete lcd;
  }

  void Display::print(char msg){
    data[row] += msg;
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
  
  void Display::print(String msg){
    data[row] += msg;
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
      Serial.println(msg);
    }
  }

  void Display::print(String msg, int row){
    if(msg != data[row]){
      if(mode == PROD || mode == DEV){
        clear(row);
        lcd->setCursor(0, row);
        lcd->print(msg);
      }
      if(mode == DEV || mode == HEADLESS) {
        Serial.println(msg);
      }
      data[row] = msg;
      this->row = row;
    }
  }

  void Display::print(Custom_char symbol){
    lcd->write(symbol);
  }

  

  void Display::scroll(){
    rowMaxLength = std::max(data[0].length(),data[1].length());
    
    if (rowMaxLength > 16 && scrolled == false){
      for (int positionCounter = 0; positionCounter < rowMaxLength - 16; positionCounter++) {
        lcd->scrollDisplayLeft();
        delay(75);
        }
      Serial.println("Scrolled screen by: " + (rowMaxLength - 16));
      scrolled = true;
    }
    else if(scrolled == true){
      for (int positionCounter = 0; positionCounter < rowMaxLength - 16; positionCounter++) {
        lcd->scrollDisplayRight();
        delay(75);
        }
      Serial.println("Scrolled screen back by: " + (rowMaxLength - 16));
      scrolled = false;
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
