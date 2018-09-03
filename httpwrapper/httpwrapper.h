#pragma once

#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>
#include "hackPSUhttp.h"
#include <ArduinoJson.h>
#define redisHost "66.71.80.160:3000" //TODO: change to actual host name

namespace hackPSU {
  class HttpImpl{
    public:
      static jawn getDataFromPin(int pin);
      static jawn assignRfidToUser(String userId, String userBandId);
      static bool entryScan(String userBandId, String locationId);
  }
}