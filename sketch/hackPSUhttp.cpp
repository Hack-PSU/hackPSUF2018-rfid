//https://techtutorialsx.com/2016/07/21/esp8266-post-requests/
#include "hackPSUhttp.h"

namespace hackPSU {
    Response* HTTP::GET(String url, int count, Headers headers[]){
    HTTPClient http;
    http.begin(url); //http Begin call

    for (int i = 0; i<count; i++){
      Headers header = headers[i];
      http.addHeader(header.headerKey,header.headerValue);
    }

    int httpCode = http.GET(); //GET call
    Response* responseInfo;
    responseInfo = new Response;
    responseInfo->payload = http.getString(); 		//getting the repsonse from the get
    responseInfo->responseCode = httpCode;

    const char* errrorString []= {"success","HTTPC_ERROR_CONNECTION_REFUSED","HTTPC_ERROR_SEND_HEADER_FAILED","HTTPC_ERROR_SEND_PAYLOAD_FAILED", "HTTPC_ERROR_NOT_CONNECTED"};
    int code = (httpCode*-1);
    String x = errrorString[code];
    responseInfo->errorStrings = x;
    
    http.end();
    return responseInfo;
  }

    Response* HTTP::GET(String url){
    HTTPClient http;
    http.begin(url); //http Begin call
    int httpCode = http.GET(); //GET call

    Response* responseInfo = new Response;

    responseInfo->payload = http.getString(); 		//getting the repsonse from the get
    responseInfo->responseCode = httpCode;
    const char* errrorString []= {"success","HTTPC_ERROR_CONNECTION_REFUSED","HTTPC_ERROR_SEND_HEADER_FAILED","HTTPC_ERROR_SEND_PAYLOAD_FAILED", "HTTPC_ERROR_NOT_CONNECTED"};
    int code = (httpCode*-1);
    String x = errrorString[code];
    responseInfo->errorStrings = x;
    
    http.end();
    return responseInfo;
  }

    Response* HTTP::POST(String url, String payload, int count, Headers headers[]){
    HTTPClient http;
    http.begin(url); //http Begin call

    for (int i = 0; i<count; i++){
      Headers header = headers[i];
      http.addHeader(header.headerKey,header.headerValue);
    }

    int httpCode = http.POST(payload); //POST call sending the payload?

    Response* responseInfo = new Response;

    responseInfo->payload = http.getString(); 		
    responseInfo->responseCode = httpCode;
    const char* errrorString []= {"success","HTTPC_ERROR_CONNECTION_REFUSED","HTTPC_ERROR_SEND_HEADER_FAILED","HTTPC_ERROR_SEND_PAYLOAD_FAILED", "HTTPC_ERROR_NOT_CONNECTED"};
    int code = (httpCode*-1);
    String x = errrorString[code];
    responseInfo->errorStrings = x;
    http.end();
    return responseInfo;
  }

    Response* HTTP::POST(String url, String payload){
      HTTPClient http;
      http.begin(url); //http Begin call
  
      int httpCode = http.POST(payload); //POST call sending the payload?
      Response* responseInfo = new Response;
  
      responseInfo->payload = http.getString(); 		
      responseInfo->responseCode = httpCode;
      const char* errrorString []= {"success","HTTPC_ERROR_CONNECTION_REFUSED","HTTPC_ERROR_SEND_HEADER_FAILED","HTTPC_ERROR_SEND_PAYLOAD_FAILED", "HTTPC_ERROR_NOT_CONNECTED"};
      int code = (httpCode*-1);
      String x = errrorString[code];
      responseInfo->errorStrings = x;
      
      http.end();
      return responseInfo;
    }
}
