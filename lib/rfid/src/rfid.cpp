#include <rfid.h>

namespace hackPSU {

  constexpr byte Scanner::DEFAULT_KEY[6];

  Scanner::Scanner(const uint8_t ssPin, const uint8_t rstPin, const byte* key) : reader(ssPin, rstPin){
    SPI.begin();
    this->reader.PCD_Init();
    this->reader.PCD_SetAntennaGain(reader.RxGain_max);
    this->key = key;
  }

  Scanner::Scanner(uint8_t ssPin, uint8_t rstPin) : Scanner(ssPin, rstPin, Scanner::DEFAULT_KEY) {}

  uint32_t Scanner::getUID_noStop(unsigned long timeout){
    byte data[4];
    uint32_t* uid;
    unsigned long time = millis();

    //Wait for new card and yield to coroutines while waiting
    while(!this->reader.PICC_IsNewCardPresent()){
      yield();
      if ((millis() - timeout > time) && timeout) {
        //timeout
        this->reader.PICC_HaltA();
        return 0;
      }
    }

    //Wait for card with data and yield to coroutines while waiting
    while(!this->reader.PICC_ReadCardSerial()){
      yield();
      if ((millis() - timeout > time) && timeout) {
        //timeout
        this->reader.PICC_HaltA();
        return 0;
      }
    }
    
    //Copy into local buffer
    for(uint8_t i = 0; i < 4; i++){
      data[i] = reader.uid.uidByte[i];
    }

    uid = (uint32_t*)data;

    return *uid;
  }


  uint32_t Scanner::getUID(unsigned long timeout){

    uint32_t uid = this->getUID_noStop(timeout);
    //Free reader resources
    this->reader.PICC_HaltA();
    return uid;
    
  }

  RfidState Scanner::getData(byte* buffer, byte size, byte blockAddr, unsigned long timeout){

    MFRC522::StatusCode status;
    RfidState retval = GOOD_RF;
    uint32_t uid = this->getUID_noStop(timeout);
    if(uid == 0) {
      retval = TIMEOUT;
      goto cleanup_read;
    }

    MFRC522::MIFARE_Key key_mf;
    for (int i = 0; i < KEY_LEN; i++)
      key_mf.keyByte[i] = key[i];

    if (reader.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 7, &key_mf, &(reader.uid)) != MFRC522::STATUS_OK) {
      retval = CRYPTO_FAIL;
      goto cleanup_read;
    }

    if ((status = reader.MIFARE_Read(blockAddr, buffer, &size)) != MFRC522::STATUS_OK) {
      retval = READ_FAIL;
      goto cleanup_read;
    }

    cleanup_read:;
    // Halt PICC
    reader.PICC_HaltA();
    // Stop encryption on PCD
    reader.PCD_StopCrypto1();
    return retval;
  }

  RfidState Scanner::setData(byte* buffer, byte size, byte blockAddr, unsigned long timeout){

    MFRC522::StatusCode status;
    RfidState retval = GOOD_RF;
    uint32_t uid = this->getUID_noStop(timeout);

    if(uid == 0) {
      retval = TIMEOUT;
      goto cleanup_write;
    }

    MFRC522::MIFARE_Key key_mf;
    for (int i = 0; i < KEY_LEN; i++)
      key_mf.keyByte[i] = key[i];

    if (reader.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, 7, &key_mf, &(reader.uid)) != MFRC522::STATUS_OK) {
      retval = CRYPTO_FAIL;
      goto cleanup_write;
    }

    if ((status = reader.MIFARE_Write(blockAddr, buffer, size)) != MFRC522::STATUS_OK) {
      retval = WRITE_FAIL;
      goto cleanup_write;
    }

    cleanup_write:;
    // Halt PICC
    reader.PICC_HaltA();
    // Stop encryption on PCD
    reader.PCD_StopCrypto1();
    return retval;
  }

}