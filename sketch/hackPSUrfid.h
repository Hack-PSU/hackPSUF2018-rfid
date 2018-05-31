#pragma once

#include <Scheduler.h>
#include <MFRC522.h>

namespace hackPSU {

  class Scanner{
    private:
      static const byte DEFAULT_KEY[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
      byte* key;
      MFRC522 reader;
    public:
      Scanner(uint8_t ssPin, uint8_t rstPin, byte* key=&DEFAULT_KEY);
      uint32_t getUID(void);
      uint32_t getData(void);
      void setData(uint32_t data);
  }

}
