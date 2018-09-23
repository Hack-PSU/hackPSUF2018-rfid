
//https://techtutorialsx.com/2016/07/21/esp8266-post-requests/
#include "hackPSUhttp.h"

namespace hackPSU {
  String HTTP::handleError(int httpCode){
    switch(httpCode){
      // List of known/common http codes:
      case 200: return "Success";
      case -1:  return "HTTPC_ERROR_CONNECTION_REFUSED";
      case -2:  return "HTTPC_ERROR_SEND_HEADER_FAILED";
      case -3:  return "HTTPC_ERROR_SEND_PAYLOAD_FAILED";
      case -4:  return "HTTPC_ERROR_NOT_CONNECTED";
      case -5:  return "HTTPC_ERROR_CONNECTION_LOST";
      case -6:  return "HTTPC_ERROR_NO_STREAM";
      case -7:  return "HTTPC_ERROR_NO_HTTP_SERVER";
      case -8:  return "HTTPC_ERROR_TOO_LESS_RAM";
      case -9:  return "HTTPC_ERROR_ENCODING";
      case -10: return "HTTPC_ERROR_STREAM_WRITE";
      case -11: return "HTTPC_ERROR_READ_TIMEOUT";
      default:
        if(httpCode >= 500) return "Server error";
        if(httpCode >= 400) return "Client error";
        if(httpCode >= 200) return "Misc. Success";
        if(httpCode >= 300) return "Redirection";
        if(httpCode >= 100) return "Info response";
        return "Unknown code";
    }
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
    responseInfo->errorMessage = handleError(httpCode);  //httpCode comes in negative

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
