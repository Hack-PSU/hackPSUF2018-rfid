#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <HackPSUlcdDisplay.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);
using namespace hackPSU;

class LcdDisplay {
  
  public:
    void printMsg(String msg) {
      lcd.setCursor(0, 1);
      lcd.print(msg);
    }
    
    void printLine(String msg) {
      lcd.setCursor(0, 1);
      lcd.print(msg);
    }

    void clearScreen(){
      lcd.clear();
    }
    
    void clearRow(int row){
      lcd.setCursor(row, 1);
      lcd.print("                ");//16 spaces
    }
}

