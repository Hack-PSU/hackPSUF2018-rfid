#pragma once 

#include <stdint.h>
#include <stdlib.h>
#include <HttpClient.h>

namespace hackPSU {
	
	struct Response{
  		String payload;
  		int responseCode;
	}responseInfo;   //declare structure obejct to return

  class HTTP{
    HTTPClient http;
    http.begin("I DONT KNOW THE IP"); //http Begin call
    int httpCode = http.GET(); //GET call

  	//httpCode will be negative (or zero) if there is an error
  	if (httpCode > 0){ 	//If the GET call was successful
      responseInfo.payload = http.getString(); 		//getting the repsonse from the get
      responseInfo.responseCode = httpCode;
  	}else{
      responseInfo.payload = http.errorToString(httpCode).c_str(); //I think this is the error information??
      responseInfo.responseCode = httpCode; //should be negative or zero?
  	}
    http.end();
  };
}




