#include "hackPSUrfid.h"

namespace hackPSU {

  Scanner::Scanner(uint8_t ssPin, uint8_t rstPin, byte* key=&DEFAULT_KEY){
    this.reader = rfid(ssPin, rstPin);
    this.key = key
  }
  
  Scanner::getUID(void){
    
  }

}

