#include "hackPSUrfid.h"

namespace hackPSU {

constexpr byte Scanner::DEFAULT_KEY[6];

  
  Scanner::Scanner(const uint8_t ssPin, const uint8_t rstPin, const byte* key) : reader(ssPin, rstPin){
    this->reader.PCD_Init();
    this->reader.PCD_SetAntennaGain(reader.RxGain_max);
    this->key = key;
  }

  Scanner::Scanner(uint8_t ssPin, uint8_t rstPin) : Scanner(ssPin, rstPin, Scanner::DEFAULT_KEY) {}
  
  uint32_t Scanner::getUID(void){
    byte data[4];
    uint32_t* uid;

    //Wait for new card and yield to coroutines while waiting
    while(!this->reader.PICC_IsNewCardPresent()){
      yield();
    }

    //Wait for card with data and yield to coroutines while waiting
    while(!this->reader.PICC_ReadCardSerial()){
      yield();
    }

    //Copy into local buffer
    for(uint8_t i = 0; i < 4; i++){
      data[i] = reader.uid.uidByte[i];
    }
    //Free reader resources
    this->reader.PICC_HaltA();

    uid = (uint32_t*)data;
    
    return *uid;
  }

  //TODO
  uint32_t Scanner::getData(void){
    return 0;
  }

  //TODO
  void Scanner::setData(uint32_t data){
    yield();
  }

}

