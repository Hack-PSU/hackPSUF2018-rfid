#pragma once 

#include <stdint.h>
#include <stdlib.h>
#include <HttpClient.h>

namespace hackPSU {
	
	struct Response {
  		String payload;
  		int responseCode;
	}responseInfo;   //declare structure obejct to return

  class HTTP{
    public:
      static Response GET(String url, String payload, String headers)
      static Response GET(String url, String payload)
      static Response POST(String url, String payload, String headers)
      static Response GET(String url, String payload)

  };
}




