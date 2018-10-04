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

      for(Menu_item &m : menu){
        m.symbol = NONE_C;
        m.key = '\0';
      }

      //creating custom characters:
      uint8_t check[8] = {0x0,0x1,0x3,0x16,0x1c,0x8,0x0};
      uint8_t erase[8] = {0x0,0x1b,0xe,0x4,0xe,0x1b,0x0};
      uint8_t upArrow[8] = {0x0, 0x4, 0xe, 0x1f, 0x4, 0x4, 0x4, 0x0};
      uint8_t downArrow[8] = {0x0, 0x4, 0x4, 0x4, 0x1f, 0xe, 0x4, 0x0};
      uint8_t backArrow[8] = {0x0, 0x4, 0x8, 0x1f, 0x9, 0x5, 0x0, 0x0};
      uint8_t lock[8] = {0xe, 0xa, 0x1f, 0x11, 0x1b, 0x1b, 0x1f, 0x0};
      uint8_t scroll[8] = {0x2, 0x1f, 0x2, 0x0, 0x8, 0x1f, 0x8, 0x0};
      
      lcd->init();
      lcd->clear();
      
      lcd->createChar(CHECK_C, check);
      lcd->home();
      lcd->createChar(UP_C, upArrow);
      lcd->home();
      lcd->createChar(DOWN_C, downArrow);
      lcd->home();
      lcd->createChar(CLEAR_C, erase);
      lcd->home();
      lcd->createChar(BACK_C, backArrow);
      lcd->home();
      lcd->createChar(LOCK_C, lock);
      lcd->home();
      lcd->createChar(SCROLL_C, scroll);
      
      lcd->backlight();
      lcd->home();//custom characters only work with lcd->home for some reason
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
  }

  void Display::print(char key1, Custom_char symbol1, char key2, Custom_char symbol2, char key3, Custom_char symbol3, char key4, Custom_char symbol4){
    if(mode == HEADLESS){
      return;
    }
    bool changed = false;
    if(menu[0].symbol != symbol1 || (menu[0].key != key1 && menu[0].symbol != NONE_C)) {
      menu[0].symbol = symbol1;
      menu[0].key = key1;
      changed = true;
    }
    if(menu[1].symbol != symbol2 || (menu[1].key != key2 && menu[1].symbol != NONE_C)) {
      menu[1].symbol = symbol2;
      menu[1].key = key2;
      changed = true;
    }
    if(menu[2].symbol != symbol3 || (menu[2].key != key3 && menu[2].symbol != NONE_C)) {
      menu[2].symbol = symbol3;
      menu[2].key = key3;
      changed = true;
    }
    if(menu[3].symbol != symbol4 || (menu[3].key != key4 && menu[3].symbol != NONE_C)) {
      menu[3].symbol = symbol4;
      menu[3].key = key4;
      changed = true;
    }

    if(changed){
      lcd->home();
      lcd->print("                ");
      lcd->home();
      for(const Menu_item &m: menu){
        print(m);
      }
    }
  }
  
  void Display::print(Menu_item control){
    if(mode == HEADLESS){
      return;
    }
    
    if(control.symbol == NONE_C){
      lcd->print("    ");
      return;
    }
    lcd->print(String(control.key));
    lcd->print(":");
    lcd->write(control.symbol);
    lcd->print(" ");
  }

  void Display::scroll(){
    String temp = data[row];
    temp += "    ";
    
    if(temp.length() > 16) {
       for (int i = 0; i<=temp.length(); i++) {
          lcd->setCursor(0, row);
          if(i == temp.length()) {
            lcd->print(temp.substring(0, 16));
          }
          else if(i + 15 >= temp.length()) {
            lcd->print(temp.substring(i) + 
                       temp.substring(0, (i+16)-temp.length()));
          } 
          else {
            lcd->print(temp.substring(i, i+16));
          }

          
          //pause 1 second every 17 scrolls left
          if((i+16 < data[row].length() && (i+1)%17 == 0) || i + 16 == data[row].length()) {
            delay(1500);
          }
          else {
            delay(100);
          }
       }
    }
  }

  void Display::clear(){
    data[0] = "";
    data[1] = "";
    if(mode != HEADLESS) {
      lcd->clear();
      for(Menu_item &m : menu){
        m.symbol = NONE_C;
        m.key = '\0';
      }
      lcd->home();
    }
  }

  void Display::clear(int row){
    if(mode != HEADLESS) {
      lcd->setCursor(0, row);
      lcd->print("                ");
      lcd->setCursor(0, row);
      if(row == 0) {
        for(Menu_item &m : menu){
          m.symbol = NONE_C;
          m.key = '\0';
        }
      }
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

#ifdef printByte(args)
#undef printByte(args)
#endif 
