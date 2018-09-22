#include "hackPSUrfid.h"

namespace hackPSU {

  constexpr byte Scanner::DEFAULT_KEY[6];
  
  Scanner::Scanner(const uint8_t ssPin, const uint8_t rstPin, const byte* key) : reader(ssPin, rstPin){
    SPI.begin();
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

  void Scanner::setUID(uint32_t uid){
    
    //Create one hacky_boi to run the uid change
   // MFRC522Hack hacky_boi(&reader);
    
    //While we haven't set the UID, yield
    //while(!hacky_boi.MIFARE_SetUid((byte*)&uid, (byte)4, true))
    //  yield();

    //Wait for new card and yield to coroutines while waiting
    while(!this->reader.PICC_IsNewCardPresent()){
      yield();
    }

    //Wait for card with data and yield to coroutines while waiting
    while(!this->reader.PICC_ReadCardSerial()){
      yield();
    }
      
    //Halt the reader so we don't break it
    byte* newUID = (byte*)&uid;
    if ( reader.MIFARE_SetUid(newUID, (byte)4, true) ) {
      Serial.println(F("Wrote new UID to card."));
    }
      
    reader.PICC_HaltA();
    return;
  }

}
