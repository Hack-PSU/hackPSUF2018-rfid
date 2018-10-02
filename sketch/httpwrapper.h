#pragma once

#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>
#include <ArduinoJson.h>
#include "hackPSUhttp.h"
#include "hackPSUconfig.h"

namespace hackPSU {
  typedef enum {SUCCESS, FAIL, TIMEOUT, REDIS_DOWN} api_response;

  class RedisData {
    public:
      char* uid;
      char* pin;
      char* name;
      char* shirtSize;
      char* diet;
      char* counter;
      char* numScans;
      RedisData();
      RedisData(
        const char *uid,
        const char *pin,
        const char *name,
        const char *shirtSize,
        const char *diet,
        const char *counter,
        const char *numScans
      );
      ~RedisData();
  };

  class HTTPImpl{
    private:
      String redisHost, apiKey;
    public:
      
      api_response getAPIKey();
      
      HTTPImpl(String host);

      /*
        Use case: The first time the user gives us their pin during regisration and we need this information
        Parameters: String pin
        Returns: uid, pin, name, shirtsize, diet restriction, counter, number of scans
        pinData is allocated by the function and should be freed upon completion by the caller
     */
      RedisData* getDataFromPin(String pin);

      /*
        Use case: The first time the bands are scanned during regisration and we need to assign the band to the user
        Parameters: String userId, String userBandId
        Returns: success or fail
     */
      api_response assignRfidToUser(String rfidCode, String pin);

      /*
        Use case: When the user is trying to scan into a workshop/event or food
        Parameters: String userBandId, String locationId
        Returns: success or fail; true is returned iff no erros occur and the response JSON field isRepeat is set to false
     */
      bool entryScan(String locationId, String rfidTag);

      Location* getLocations(int &len);

  };
}
