#pragma once

#include <stdint.h>
#include <MFRC522.h>
#include <SPI.h>

#include "hackPSUconfig.h"

#define KEY_LEN 6
#define READ_BUFFER 32
#define WRITE_BUFFER 16
#define KEY_BLOCK 4

namespace hackPSU {

  typedef enum {GOOD_RF, CRYPTO_FAIL, WRITE_FAIL, READ_FAIL, TIMEOUT} RfidState;

  class Scanner{
    private:
      constexpr static byte DEFAULT_KEY[KEY_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
      const byte* key;
      MFRC522 reader;

      uint32_t getUID_noStop(unsigned long timeout);
      
    public:
      Scanner(uint8_t ssPin, uint8_t rstPin, const byte* key);
      Scanner(uint8_t ssPin, uint8_t rstPin);

      //Timeout is negated if timeout==0
      uint32_t getUID(unsigned long timeout);
      RfidState getData(byte* buffer, byte size, byte blockAddr, unsigned long timeout);      
      RfidState setData(byte* buffer, byte size, byte blockAddr, unsigned long timeout); 

  };

}
