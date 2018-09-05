#pragma once 

#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>
#include <HttpClient.h>

namespace hackPSU {
  typedef struct Response {
    String payload;
    int responseCode;
  } Response;   //declare structure obejct to return

  typedef struct Headers{   //does this belong here??
    String headerKey;
    String headerValue;
  } Headers;

  class HTTP{
    public:
      /*
        Use case: When a GET request must be made with headers
        Returns: payload and response code
     */
      static Response* GET(String url, int count, Headers headers[]);

      /*
        Use case: When a GET request must be made without headers
        Returns: payload and response code
     */
      static Response* GET(String url);

      /*
        Use case: When a POST request must be made with headers
        Returns: payload and response code
     */
      static Response* POST(String url, String payload, int count, Headers headers[]);
      
      /*
        Use case: When a POST request must be made without headers
        Returns: payload and response code
     */
      static Response* POST(String url, String payload);

  };
}