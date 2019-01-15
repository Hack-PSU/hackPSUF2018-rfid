#pragma once

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

#include "hackPSUconfig.h"

namespace hackPSU {

  typedef struct Response {
    String payload;
    int code;
  } Response;

  namespace API{
    typedef enum {SUCCESS, FAIL, TIMEOUT, REDIS_DOWN} Response;
  }

  typedef struct Header{
    String key;
    String value;
  } Header;

  typedef enum {apiapi_responseapi_responseapi_response_response
    GET, POST
  } Methods_t;

  class WiFi{
    public:

      JsonObject& payload;
      JsonObject& headers;

      API::Response getStatus(int httpCode);

      WiFi();

      /*
        Use case: Any request
        Parameters: Methods_t method - GET/POST supported currently
                    String url       - API route, Default "/"
                    int headerCount  - Number of headers in use, Default 0
                    Headers headers  - All headers required in request, Default nullptr 
                    String paylod    - Payload for POST request, Default ""
        Returns: Response pointer (job of caller to free memory)
     */
      static Response* request(Methods_t method);
      
      /*
        Use case: Get specific values from a request
        Parameters: String payload     - Returned data
                    JsonObject &result - JSON object of desired data
        Returnes: Fills out result given data (by reference)
      */
      static void parse(String payload,      JsonObject& output);
      
      /*
        Use case: Decodeing return code
        Parameters: Return code
        Returns: Human understandable return code
      */
      static String errorMessage(int httpCode);

      API::Response getApiKey();
      API::Response getDataFromPin(String pin, JsonObject& output);
      API::Response assignRfidToUser(String rfidCode, String pin, JsonObject& output);
      API::Response entryScan(String locationID, String rfidCode, JsonObject& output);
      API::Response getLocations(String rfidCode, String pin, JsonObject& output);



    private:
      HTTPClient http;      // static object for all HTTP transactions
      String apiKey;

      static void parse(JsonObject& payload, JsonObject& output);
      static API::Response parseCode(int code);

      #if defined(STATIC)
        StaticJsonBuffer<JSON_OBJECT_SIZE(5)> bf_header;
        StaticJsonBuffer<JSON_OBJECT_SIZE(75)> bf_payload;
      #elif defined(DYNAMIC)
        DynamicJsonBuffer bf_header;
        DynamicJsonBuffer bf_payload;
      #endif
  };
}
