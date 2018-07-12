//https://techtutorialsx.com/2016/07/21/esp8266-post-requests/
#include <hackPSUhttp.h>

namespace hackPSU {
  	Response::GET(String url, String payload, struct headers){
  		HTTPClient http;
   		http.begin(url); //http Begin call
    		http.addHeader(headerInfo.header);
		int httpCode = http.GET(); //GET call

  		//httpCode will be negative (or zero) if there is an error

  		if (httpCode > 0){ 	//If the GET call was successful
      			responseInfo.payload = http.getString(); 		//getting the repsonse from the get
      			responseInfo.responseCode = httpCode;
  		}else{
     			responseInfo.payload = http.errorToString(httpCode); //I think this is the error information??
     			responseInfo.responseCode = httpCode; //should be negative or zero?
  		}
    		http.end();
  	}

  	Response::GET(String url, String payload){
    		HTTPClient http;
    		http.begin(url); //http Begin call
   		int httpCode = http.GET(); //GET call

  		//httpCode will be negative (or zero) if there is an error
  		if (httpCode > 0){ 	//If the GET call was successful
     			responseInfo.payload = http.getString(); 		//getting the repsonse from the get
      			responseInfo.responseCode = httpCode;
  		}else{
      			responseInfo.payload = http.errorToString(httpCode); //I think this is the error information??
      			responseInfo.responseCode = httpCode; //should be negative or zero?
  		}
    		http.end();
  	}

  	Response::POST(String url, String payload, struct headers){
    		HTTPClient http;
    		http.begin(url); //http Begin call
    		http.addHeader(headerInfo.header);

    		int httpCode = http.POST(payload); //POST call sending the payload?

  		//httpCode will be negative (or zero) if there is an error
  		if (httpCode){ 	//If the POST call was successful
      			responseInfo.payload = http.getString(); 		
      			responseInfo.responseCode = httpCode;
  		}else{
      			responseInfo.payload = http.errorToString(httpCode);
      			responseInfo.responseCode = httpCode;
  		}
    		http.end();
  	}

  	Response::POST(String url, String payload){
    		HTTPClient http;
    		http.begin(url); //http Begin call

    		int httpCode = http.POST(payload); //POST call sending the payload?

  		//httpCode will be negative (or zero) if there is an error
  		if (httpCode){ 	//If the POST call was successful
      			responseInfo.payload = http.getString(); 		
      			responseInfo.responseCode = httpCode;
  		}else{
      			responseInfo.payload = http.errorToString(httpCode);
      			responseInfo.responseCode = httpCode;
  		}
    		http.end();
  	}
