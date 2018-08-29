//https://techtutorialsx.com/2016/07/21/esp8266-post-requests/
#include "hackPSUhttp.h"

namespace hackPSU {
	static Response* HTTP::GET(String url, int count, Headers headers[]){
		HTTPClient http;
   		http.begin(url); //http Begin call

        for (int i = 0; i<count, i++){
			Headers headers = headers[i];
        	http.addHeader(headers.headerKey,headers.headerValue);
        }

		int httpCode = http.GET(); //GET call

		Response* responseInfo;
        responseInfo = new Response;

        responseInfo->payload = http.getString(); 		//getting the repsonse from the get
        responseInfo->responseCode = httpCode;

    	http.end();
    	return responseInfo;
	}

  	static Response* HTTP::GET(String url){
    	HTTPClient http;
    	http.begin(url); //http Begin call
   		int httpCode = http.GET(); //GET call

   		Response* responseInfo;
        responseInfo = new Response;

     	responseInfo->payload = http.getString(); 		//getting the repsonse from the get
      responseInfo->responseCode = httpCode;

    	http.end();
    	return responseInfo;
  	}

  	static Response* HTTP::POST(String url, String payload, int count, Headers headers[]){
    	HTTPClient http;
    	http.begin(url); //http Begin call
    	
        for (int i = 0; i<count, i++){
			Headers headers = headers[i];
			http.addHeader(headers.headerKey,headers.headerValue);
         }

    	int httpCode = http.POST(payload); //POST call sending the payload?

    	Response* responseInfo;
        responseInfo = new Response;

        responseInfo->payload = http.getString(); 		
        responseInfo->responseCode = httpCode;

    	http.end();
    	return responseInfo;
  	}

  	static Response* HTTP::POST(String url, String payload){
    	HTTPClient http;
    	http.begin(url); //http Begin call

    	int httpCode = http.POST(payload); //POST call sending the payload?

    	Response* responseInfo;
        responseInfo = new Response;

        responseInfo->payload = http.getString(); 		
        responseInfo->responseCode = httpCode;

    	http.end();
    	return responseInfo;
  	}
}
