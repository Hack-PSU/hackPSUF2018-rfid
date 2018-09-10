#pragma once

#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>
#include <ArduinoJson.h>
#include "hackPSUhttp.h"

namespace hackPSU {
  typedef struct redisData{
    const char* uid;
    const char* pin;
    const char* name;
    const char* shirtSize;
    const char* diet;
    const char* counter;
    const char* numScans;
  } redisData;

  class HTTPImpl{
    private:
      String redisHost;
    public:
      HTTPImpl(String host): redisHost(host) {}

      /*
        Use case: The first time the user gives us their pin during regisration and we need this information
        Parameters: String pin 
        Returns: uid, pin, name, shirtsize, diet restriction, counter, number of scans
        pinData is allocated by the function and should be freed upon completion by the caller
     */
      redisData* getDataFromPin(String pin);
      
      /*
        Use case: The first time the bands are scanned during regisration and we need to assign the band to the user
        Parameters: String userId, String userBandId
        Returns: success or fail
     */
      bool assignRfidToUser(String userId, String userBandId);
      
      /*
        Use case: When the user is trying to scan into a workshop/event
        Parameters: String userBandId, String locationId
        Returns: success or fail (they can enter or not)
     */
      bool entryScan(String userBandId, String locationId);
  };
}
