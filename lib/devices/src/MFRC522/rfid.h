#pragma once

#include <stdint.h>
#include <MFRC522.h>
#include <SPI.h>

#include <config.h>
#include <core.hpp>

#define KEY_LEN 6
#define READ_BUFFER 32
#define WRITE_BUFFER 16
#define KEY_BLOCK 4

namespace hackPSU {

  typedef enum {GOOD_RF, CRYPTO_FAIL, WRITE_FAIL, READ_FAIL, TIMEOUT} RfidState;

  class Scanner: public MFRC522, public Readable<uint32_t>{
    private:
      constexpr static byte DEFAULT_KEY[KEY_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
      constexpr static byte master_key[WRITE_BUFFER] = MASTER_KEY;
      const byte* key;
      
      uint32_t scan();
      byte read_buffer[READ_BUFFER];
    public:
      Scanner(char* name, uint8_t ssPin, uint8_t rstPin, const byte* key);
      Scanner(char* name, uint8_t ssPin, uint8_t rstPin);

      //Timeout is negated if timeout==0
      uint32_t getUID(unsigned long timeout, bool stop = true);
      bool isMaster();
      RfidState getData(byte* buffer, byte size, byte blockAddr, unsigned long timeout, bool getuid = true);      
      RfidState setData(byte* buffer, byte size, byte blockAddr, unsigned long timeout, bool getuid = true); 
  };

}
