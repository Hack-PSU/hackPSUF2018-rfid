#pragma once

#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>
#include "hackPSUhttp.h"

namespace hackPSU {
  class HttpImpl{
  public:
    static jawn getDataFromPin(int pin);
    static jawn assignRfidToUser(String userId, String userBandId);
    static bool entryScan(String userBandId, String locationId);
  }
}