#include "hackPSUrfid.h"

namespace hackPSU {

  Scanner::Scanner(uint8_t ssPin, uint8_t rstPin, byte* key=&DEFAULT_KEY){
    this.reader = mfrc522(ssPin, rstPin);
    this.reader.PCD_Init();
    this.reader.PCD_SetAntennaGain(reader.RxGain_max);
    this.key = key
  }
  
  uint32_t Scanner::getUID(void){
    byte data[4];
    uint32_t* uid;

    //Wait for new card and yield to coroutines while waiting
    while(!this.reader.PICC_IsNewCardpresent()){
      yield();
    }

    //Wait for card with data and yield to coroutines while waiting
    while(!this.reader.PICC_ReadCardSerial()){
      yield();
    }

    //Copy into local buffer
    for(uint8_t i = 0; i < 4; i++){
      data[i] = reader.uid.uidByte[i];
    }
    //Free reader resources
    this.reader.PICC_HaltA();

    uid = (uint32_t*)data;
    
    uid = sprintf("0x%x%x%x%x", data[0], data[1], data[2], data[2]);
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

