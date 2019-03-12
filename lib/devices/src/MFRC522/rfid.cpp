#include "rfid.h"

namespace hackPSU {

  constexpr byte Scanner::DEFAULT_KEY[KEY_LEN];
  constexpr byte Scanner::master_key[WRITE_BUFFER];

  Scanner::Scanner(char* name, const uint8_t ssPin, const uint8_t rstPin, const byte* key) : 
    MFRC522(ssPin, rstPin),
    Readable<uint32_t>::Readable(name)
  {
    SPI.begin();
    this->PCD_Init();
    this->PCD_SetAntennaGain(this->RxGain_max);
    this->key = key;
  }

  Scanner::Scanner(char* name, uint8_t ssPin, uint8_t rstPin) : 
    Scanner(name, ssPin, rstPin, Scanner::DEFAULT_KEY)
  {

  }

  uint32_t Scanner::getUID(unsigned long timeout, bool stop){
    byte data[4];
    uint32_t* uid;
    unsigned long time = millis();

    //Wait for new card and yield to coroutines while waiting
    while(!this->PICC_IsNewCardPresent()){
      yield();
      if ((millis() - timeout > time) && timeout) {
        //timeout
        this->PICC_HaltA();
        return 0;
      }
    }

    //Wait for card with data and yield to coroutines while waiting
    while(!this->PICC_ReadCardSerial()){
      yield();
      if ((millis() - timeout > time) && timeout) {
        //timeout
        this->PICC_HaltA();
        return 0;
      }
    }
    
    //Copy into local buffer
    for(uint8_t i = 0; i < 4; i++){
      data[i] = this->uid.uidByte[i];
    }

    if(stop){
      this->PICC_HaltA();
    }

    uid = (uint32_t*)data;

    return *uid;
  }

  RfidState Scanner::getData(byte* buffer, byte size, byte blockAddr, unsigned long timeout, bool getuid){

    RfidState retval = GOOD_RF;
    if(getuid && getUID(timeout, false) == 0) {
      retval = TIMEOUT;
    } else {
      MFRC522::MIFARE_Key key_mf;
      for (int i = 0; i < KEY_LEN; i++)
        key_mf.keyByte[i] = key[i];

      if (this->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 7, &key_mf, &(this->uid)) != MFRC522::STATUS_OK) {
        retval = CRYPTO_FAIL;
      } else if ((this->MIFARE_Read(blockAddr, buffer, &size)) != MFRC522::STATUS_OK) {
        retval = READ_FAIL;
      }
    }

    // Halt PICC
    this->PICC_HaltA();
    // Stop encryption on PCD
    this->PCD_StopCrypto1();
    return retval;
  }

  RfidState Scanner::setData(byte* buffer, byte size, byte blockAddr, unsigned long timeout, bool getuid){

    RfidState retval = GOOD_RF;
    if(getuid && getUID(timeout, false) == 0) {
      retval = TIMEOUT;
    } else {
      MFRC522::MIFARE_Key key_mf;
      for (int i = 0; i < KEY_LEN; i++)
        key_mf.keyByte[i] = key[i];

      if (this->PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, 7, &key_mf, &(this->uid)) != MFRC522::STATUS_OK) {
        retval = CRYPTO_FAIL;
      } else if ((this->MIFARE_Write(blockAddr, buffer, size)) != MFRC522::STATUS_OK) {
        retval = WRITE_FAIL;
      }
    }

    // Halt PICC
    this->PICC_HaltA();
    // Stop encryption on PCD
    this->PCD_StopCrypto1();
    return retval;
  }

  uint32_t Scanner::scan(){
    uint32_t ID = getUID(250, false);
    if(ID != 0 && getData(read_buffer, READ_BUFFER, KEY_BLOCK, SCAN_TIMEOUT, false) != GOOD_RF){
      return 0;
    }
    return ID;
  }

  bool Scanner::isMaster(){
    for(int i = 0; i < WRITE_BUFFER; ++i) {
      if(read_buffer[i] != master_key[i]) {
        Serial.println("Not a master wristband");
        return false;
      }
    }
    return true;
  }

}
