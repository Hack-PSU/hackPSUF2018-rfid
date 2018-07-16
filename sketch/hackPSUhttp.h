#pragma once 

#include <stdint.h>
#include <stdlib.h>
#include <HttpClient.h>

namespace hackPSU {
  
  typedef struct Response {
      String payload;
      int responseCode;
  }Response;   //declare structure obejct to return

  typedef struct Headers{   //does this belong here??
       String headerInformation;
  }Headers;

  class HTTP{
    public:
    /**
     * TODO: Add docstring explaining these parameters
     */
      static Response GET(String url, String headers)
      static Response GET(String url)
      static Response POST(String url, String payload, String headers)
      static Response GET(String url, String payload)

  };
}
