
//https://techtutorialsx.com/2016/07/21/esp8266-post-requests/
#include "hackPSUhttp.h"

namespace hackPSU {
  String HTTP::handleError(int httpCode){
    const char* errorString []= {"success","HTTPC_ERROR_CONNECTION_REFUSED","HTTPC_ERROR_SEND_HEADER_FAILED","HTTPC_ERROR_SEND_PAYLOAD_FAILED", "HTTPC_ERROR_NOT_CONNECTED"};

    return errorString[(httpCode*(-1))];
  }

  Response* HTTP::GET(String url, int headerCount, Headers headers[]){
    HTTPClient http;
    http.begin(url); //http Begin call

    for (int i = 0; i<headerCount; i++){
      Headers header = headers[i];
      http.addHeader(header.headerKey,header.headerValue);
    } /*end for loop*/

    int httpCode = http.GET(); //GET call
    Response* responseInfo = new Response;

    responseInfo->payload = http.getString();
    responseInfo->responseCode = httpCode;
    responseInfo->errorMessage = handleError(httpCode);  //httpCode comes in negative

    http.end();
    return responseInfo;
  }/* end HTTP::GET*/

  Response* HTTP::GET(String url){
    HTTPClient http;
    http.begin(url); //http Begin call

    int httpCode = http.GET(); //GET call
    Response* responseInfo = new Response;

    responseInfo->payload = http.getString();
    responseInfo->responseCode = httpCode;
    responseInfo->errorMessage = handleError(httpCode);  //httpCode comes in negative

    http.end();
    return responseInfo;
  }/* end HTTP::GET*/

  Response* HTTP::POST(String url, String payload, int headerCount, Headers headers[]){
    HTTPClient http;
    http.begin(url); //http Begin call

    Serial.println("HEAD");
    for (int i = 0; i<headerCount; i++){
      Headers header = headers[i];
      Serial.println(header.headerKey + ":" + header.headerValue);
      http.addHeader(header.headerKey,header.headerValue);
    }/* end for loop*/

    int httpCode = http.POST(payload);

    Response* responseInfo = new Response;


    responseInfo->payload = http.getString();
    responseInfo->responseCode = httpCode;
    responseInfo->errorMessage = httpCode < 0 ? handleError(httpCode) : "Success";  //httpCode comes in negative

    http.end();
    return responseInfo;
  }/* end HTTP::POST*/

  Response* HTTP::POST(String url, String payload){
      HTTPClient http;
      http.begin(url); //http Begin call

      int httpCode = http.POST(payload);
      Response* responseInfo = new Response;

      responseInfo->payload = http.getString();
      responseInfo->responseCode = httpCode;
      responseInfo->errorMessage = handleError(httpCode);  //httpCode comes in negative

      http.end();
      return responseInfo;
    }/* end HTTP::POST*/
}
