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

  class Scanner{
    private:
      constexpr static byte DEFAULT_KEY[KEY_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
      const byte* key;
      MFRC522 reader;
    public:
      Scanner(uint8_t ssPin, uint8_t rstPin, const byte* key);
      Scanner(uint8_t ssPin, uint8_t rstPin);

      uint32_t getUID(void);
      void getData(byte* buffer, byte size, byte blockAddr);      
      void setData(byte* buffer, byte size, byte blockAddr); 

  };

}
