#pragma once 

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

namespace hackPSU {
  typedef struct Response {
    String payload;
    String errorMessage;
    int responseCode;
  } Response;   //declare structure obejct to return

  typedef struct Headers{
    String headerKey;
    String headerValue;
  } Headers;
  
  class HTTP{
    public:
      /*
        Use case: When a GET request must be made with headers
        Parameters: String url (redis url), int count (amount of headers), Headers headers 
        Returns: payload and response code
        Response is allocated by the function and should be freed upon completion by the caller
     */
      static Response* GET(String url, int count, Headers headers[]);

      /*
        Use case: When a GET request must be made without headers
        Parameters: String url (redis url)
        Returns: payload and response code
        Response is allocated by the function and should be freed upon completion by the caller
     */
      static Response* GET(String url);

      /*
        Use case: When a POST request must be made with headers
        Parameters: String url (redis url), String payload (content going to redis), int count (amount of headers), Headers headers 
        Returns: payload and response code
        Response is allocated by the function and should be freed upon completion by the caller
     */
      static Response* POST(String url, String payload, int count, Headers headers[]);
      
      /*
        Use case: When a POST request must be made without headers
        Parameters: String url (redis url)
        Returns: payload and response code
        Response is allocated by the function and should be freed upon completion by the caller
     */
      static Response* POST(String url, String payload);

      static String handleError(int httpCode);

  };
}
