// Henry's Bench
//Checking to ensure you can connect ESP-12E to a router
     
#include <MFRC522/rfid.h>

hackPSU::Scanner *scanner;    

void setup() {
  delay(2000);
  Serial.begin(9600);

  Serial.println("Begin RFID test");  

  scanner = new hackPSU::Scanner("scanner", D8, D4);
}

void loop() {
  uint32_t uid = scanner->read();
  if(uid != 0){
    Serial.print("UID: ");
    Serial.println(uid);
    if(scanner->isMaster()){
      Serial.println("Master band found... clearing key");
      byte write_buffer[WRITE_BUFFER] = {0};
      switch(scanner->setData(write_buffer, WRITE_BUFFER, KEY_BLOCK, SCAN_TIMEOUT)){
        case hackPSU::CRYPTO_FAIL:
          Serial.println("Crypto Fail");
          break;
        case hackPSU::WRITE_FAIL:
          Serial.println("Write Fail");
          break;
        case hackPSU::TIMEOUT:
          Serial.println("Timeout");
          break;
        case hackPSU::GOOD_RF:
          Serial.println("Successfull write");
        default:
          Serial.println("Unknown error");
      }
    } else {
      Serial.println("Not a master band.. setting key");
      byte write_buffer[WRITE_BUFFER] = MASTER_KEY;
      switch(scanner->setData(write_buffer, WRITE_BUFFER, KEY_BLOCK, SCAN_TIMEOUT)){
        case hackPSU::CRYPTO_FAIL:
          Serial.println("Crypto Fail");
          break;
        case hackPSU::WRITE_FAIL:
          Serial.println("Write Fail");
          break;
        case hackPSU::TIMEOUT:
          Serial.println("Timeout");
          break;
        case hackPSU::GOOD_RF:
          Serial.println("Successfull write");
        default:
          Serial.println("Unknown error");
      }
    }


  }
}