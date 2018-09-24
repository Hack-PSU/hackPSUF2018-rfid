
#include "hackPSUkeypad.h"

namespace hackPSU{
  void Keypad::pulse(bool data, int space){
    digitalWrite(sig, data ? HIGH : LOW);
    digitalWrite(clk, HIGH);
    delay(space / 2);
    digitalWrite(clk, LOW);
    delay(space / 2);
  }

  Keypad::Keypad(int KPD_SRC, int KPD_CLK, int KPD_SIG, Display* display){
    pin = KPD_SRC;
    clk = KPD_CLK;
    sig = KPD_SIG;
    this->display = display;
    pinMode(clk, OUTPUT);
    pinMode(sig, OUTPUT);
    pinMode(pin, INPUT);
    clearSR();
  }

  char Keypad::readKeypad(){
    // assumed that the shift registor has been cleared
    char key = 'x'; // x is the default return value, only returned if no keypress is detected
    int i = 0;
  
    for(int i = 0; i < 4 && key == 'x'; i++){
      pulse(i == 0, READ_PERIOD);  
      int tmp = analogRead(pin);
      if( tmp > 800 ) key = keys[0][3-i];       // constants refers to analog value when key is pressed
      else if( tmp > 450 ) key = keys[1][3-i];  // The actual value fluxuates minimally so there is room
      else if( tmp > 320 ) key = keys[2][3-i];  // for this behavior
      else if( tmp > 50)   key = keys[3][3-i];
      //Serial.println(String(tmp) + " - " + key);
    }
    clearSR(); // reset shift registor after every read
    return key;
  }

  #define SPACE 5
  char Keypad::getKeyPress(int stable, unsigned long timeout) {
    char val;
    unsigned long start = millis();
    
    val = readKeypad();
    delay(SPACE);
    int i;
    for( i = 0; i <= stable && start + timeout > millis(); i++) {
      char temp = readKeypad();
      if( i == stable && val != 'x') {
        return(val);   
      } else if( temp != val ) {
        val = temp;
        i = 0;
      }  
      delay(SPACE);
    }
    return('z');
  }

  char Keypad::getUniqueKey(unsigned long timeout){
    unsigned long start = millis();
    char val = getKeyPress(1, timeout);  // 500 ms timeout
    Serial.println("Key pressed: " + val);
    if(val != 'z'){
      Serial.print(val);
      while( getKeyPress(1, start + timeout - millis()) == val && start + timeout > millis() ) { // check every 50ms
        Serial.print('.');
        delay(50);
      }
      Serial.print('\n');
    }
    return(start + timeout < millis() ? 't' : val);
  }

  String Keypad::getPin(int maxLen, const char clr, const char submit, int timeout){
    unsigned long start = millis();
    String pin = "";
    for(int i = 0; i < maxLen && start + timeout > millis(); i++){
      char key = getUniqueKey(start + timeout - millis());
      if(key == clr){
        display->clear();
        pin = "";
        i = 0;
      } else if( key == submit) {
        return pin;
      } else if ( key >= 'A' && key <= 'D'){
        return String(key);
      }else if( key != 't' && key != 'z') {
        display->print(key);
        pin += key;
        start = millis(); // timeout is from the last pressed key
      } else {
        i--;
        yield();
      }
    }
    if( start + timeout < millis() ){
      Serial.println("Timeout in getPin");
      return "timeout";
    } else {
      return pin;
    }
  }
  
}

