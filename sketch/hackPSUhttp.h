#pragma once 

#include <stdint.h>
#include <stdlib.h>
#include <HttpClient.h>

namespace hackPSU {
	
	struct Response {
  		String payload;
  		int responseCode;
	}responseInfo;   //declare structure obejct to return
	
	struct headers{		//does this belong here??
		String header;
	}headerInfo;
	
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




