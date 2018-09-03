#pragma once

#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>
#include <ArduinoJson.h>
#include "hackPSUhttp.h"

namespace hackPSU {
  class HttpImpl{
  private:
    String redisHost;
    public:
      constructor(String host): redisHost(host);
      jawn getDataFromPin(int pin);
      jawn assignRfidToUser(String userId, String userBandId);
      bool entryScan(String userBandId, String locationId);
  }
}
