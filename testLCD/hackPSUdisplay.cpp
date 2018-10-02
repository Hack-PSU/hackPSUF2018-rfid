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
      
      lcd->init();
      lcd->clear();
      lcd->backlight();
      lcd->home();//custom characters only work with lcd->home for some reason
    }

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
         if(msg.length() > 16){
            lcd->print(msg.substring(0,16));
          } else {
            lcd->print(msg);
          }
      }
      if(mode == DEV || mode == HEADLESS) {
        Serial.println(msg);
      }
      data[row] = msg;
      this->row = row;
    }
  }

  void Display::print(Custom_char symbol){
    if(mode == PROD || mode == DEV) {
      lcd->write(symbol);
    }
    if(mode == DEV || mode == HEADLESS) {
        Serial.println("Printed symbol: " + String(symbol));
    }
  }

  void Display::print(char msg, Custom_char symbol){
    if(mode == PROD || mode == DEV) {
      lcd->print(String(msg));
      lcd->print(":");
      lcd->write(symbol);
      lcd->print(" ");
    }
    if(mode == DEV || mode == HEADLESS) {
        Serial.println(String(msg) + ": " + String(symbol));
    }
    
  }

  void Display::scroll(){
    if(data[row].length() > 16) {
       Serial.println("Started scrolling");
       for (int i = 0; i<=data[row].length(); i++) {
          lcd->setCursor(0, row);
          if(i == data[row].length()) {
            lcd->print(data[row].substring(0, 15));
            Serial.println("Finished Scrolling ");
          }
          else if(i + 15 >= data[row].length()) {
            lcd->print(data[row].substring(i) + 
                       data[row].substring(0, (i+15)-data[row].length()));
          } 
          else {
            lcd->print(data[row].substring(i, i+15));
          }
          //pause 1 second every 17 scrolls left
          if((i+1)%17 == 0) {
            delay(1000);
            Serial.println("Scrolled ");
          }
          else {
            delay(150);
          }
       }
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
    if(row == 0) {
      lcd->home();
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
