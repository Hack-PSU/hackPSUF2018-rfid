#pragma once

#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>
#include <ArduinoJson.h>
#include "hackPSUhttp.h"

namespace hackPSU {
  typedef struct jawn{
    String uid;
    String givenPin;
    String name;
    String shirtSize;
    String diet;
    String counter;
    String numScans;
  } jawn;

  class HttpImpl{
    private:
      String redisHost;
    public:
      constructor(String host): redisHost(host);
      jawn* getDataFromPin(int pin);
      bool assignRfidToUser(String use``rId, String userBandId);
      bool entryScan(String userBandId, String locationId);
  }
}
