#include "hackPSUstatemachine.h"

namespace hackPSU {

  Box::Box(String redis_addr, const char* ssid, const char* password, Mode_e mode, const byte* band_key){
    scanner = new Scanner(RFID_SS, RFID_RST);
    http =    new HTTPImpl(redis_addr);
    display = new Display(mode);
    keypad =  new Keypad(KPD_SRC, KPD_CLK, KPD_SIG, display->clear, display->print);
  }
  
  Box::~Box(){
    delete scanner;
    delete keypad;
    delete http;
    delete display;
    Serial.println("Deleted all Box members");
  }

  void Box::scan(void){

    char input = keypad->readKeypad();

    switch(input){
      //B -> escape up
      case 'B':
        state = MENU;
        return;
      //C -> escape out
      case 'C':
        state = INIT;
        return;
      //Normal behavior = scan band
      default:
        Serial.println("Please scan wristband");
        uint32_t uid = scanner->getUID();
        char lid_buffer[10] = {0};
        char uid_buffer[10] = {0};
        Serial.println(uid);
        if (uid != last_scan) {
          if(http->entryScan(itoa(lid,lid_buffer, 10), itoa(uid, uid_buffer, 10))){
            //TODO LCD->print Allow
            Serial.println("Allow");
          } else {
            //TODO LCD->print Deny
            Serial.println("Deny");
          }
          last_scan = uid;
        }
        //No state transition
    }
        
  }

  void Box::duplicate(void){
    scanner->setUID(GOLDEN_KEY);
    state = MENU;
  }

  
    
}
