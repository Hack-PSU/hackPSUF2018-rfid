#pragma once

#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>
#include <ArduinoJson.h>
#include "hackPSUhttp.h"

namespace hackPSU {
  typedef struct redisData{
    String uid;
    String pin;
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
      
      /*
      (I'm not entirely sure what a docstring looks like)
        Use case: The first time the user gives us their pin during regisration and we need this information
        Returns: uid, pin, name, shirtsize, diet restriction, counter, number of scans
     */
      redisData* getDataFromPin(int pin);
      
      /*
        Use case: The first time the bands are scanned during regisration and we need to assign
                  the band to the user
        Returns: success or fail
     */
      bool assignRfidToUser(String userId, String userBandId);
      
      /*
        Use case: When the user is trying to scan into a workshop/event
        Returns: success or fail (they can enter or not)
     */
      bool entryScan(String userBandId, String locationId);
  }
}
