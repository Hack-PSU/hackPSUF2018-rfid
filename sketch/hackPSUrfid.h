#pragma once

#include <stdint.h>
#include <ESP8266WiFi.h>
#include <MFRC522.h>

namespace hackPSU {

  class Scanner{
    private:
      constexpr static byte DEFAULT_KEY[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
      const byte* key;
      MFRC522 reader;
    public:
      Scanner(uint8_t ssPin, uint8_t rstPin, const byte* key);
      Scanner(uint8_t ssPin, uint8_t rstPin);
      
      uint32_t getUID(void);
      void setUID(uint32_t uid);
      
  };

}
