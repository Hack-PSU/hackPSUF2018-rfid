#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#if defined(OTA_PASSWORD) && defined(OTA_PASSWORD_HASH)
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#endif

#include <config.h>

// Preform checks for used macros
#ifndef API_VERSION
#error Macro, API_VERSION, not set
#endif

#ifndef MASTER_KEY
#error Macro, MASTER_KEY, not set
#endif

#ifndef NETWORK_PASSWORD
#error Macro, NETWORK_PASSWORD, not set
#endif

#ifndef NETWORK_SSID
#error Macro, NETWORK_SSID, not set
#endif

#if !defined(OTA_PASSWORD) && !defined(OTA_PASSWORD_HASH)
#warning OTA uploads not enabled, define OTA_PASSWORD and OTA_PASSWORD_HASH to allow OTA uplaoding.
#endif


#if defined(DYNAMIC_BUFFER)
  #define MAKE_BUFFER(obj_size, arr_size) DynamicJsonBuffer
#else
  #define MAKE_BUFFER(obj_size, arr_size) StaticJsonBuffer<JSON_OBJECT_SIZE(obj_size)+JSON_ARRAY_SIZE(arr_size)>
#endif 

namespace hackPSU {

  namespace API{
    typedef enum {
      SUCCESS, FAIL, TIMEOUT, REDIS_DOWN, OUTDATED
    } Response;

    typedef enum {
      GET, POST
    } Method;
  }

  typedef struct Response {
    String payload;
    int code;

    operator String() const{
      switch(code){
        // List of known/common http codes:
        case 200: return F("Success");
        case 401: return F("Unauthorized");
        case 404: return F("Not Found");
        case 500: return F("Internal Server Error");
        case -1:  return F("HTTPC_ERROR_CONNECTION_REFUSED");
        case -2:  return F("HTTPC_ERROR_SEND_HEADER_FAILED");
        case -3:  return F("HTTPC_ERROR_SEND_PAYLOAD_FAILED");
        case -4:  return F("HTTPC_ERROR_NOT_CONNECTED");
        case -5:  return F("HTTPC_ERROR_CONNECTION_LOST");
        case -6:  return F("HTTPC_ERROR_NO_STREAM");
        case -7:  return F("HTTPC_ERROR_NO_HTTP_SERVER");
        case -8:  return F("HTTPC_ERROR_TOO_LESS_RAM");
        case -9:  return F("HTTPC_ERROR_ENCODING");
        case -10: return F("HTTPC_ERROR_STREAM_WRITE");
        case -11: return F("HTTPC_ERROR_READ_TIMEOUT");
        default:
          if(code >= 500) return "Server error - " + String(code);
          if(code >= 400) return "Client error - " + String(code);
          if(code >= 300) return "Redirectio - n" + String(code);
          if(code >= 200) return "Success - " + String(code);
          if(code >= 100) return "Informational response - " + String(code);
          return "Unknown code - " + String(code);
      }
    }
    operator API::Response() const{
      switch(code){
        case 401:
        case 404:
        case 409: 
            return API::FAIL;
        case 200: 
            return API::SUCCESS;
        case 500: 
            return API::TIMEOUT;
        default:
            return API::REDIS_DOWN;
      }
    }
    operator bool() const {
      return code >= 200 && code < 300;
    }
  } Response;



  /**
   * Only one Request per HTTP request
   * */


  class Network{
    public:
      Network(String host);
      void beginRequest(API::Method method, String route);

      Response* completeRequest();

      bool addPayload(String key, String value);
      bool addHeader(String key, String value);

      API::Response getApiKey();

      bool connected();
      #if defined(OTA_PASSWORD) && defined(OTA_PASSWORD_HASH)
      void enableOTA();
      void handleOTA();
      #endif


    private:
      class Request{
      friend class Network;
      private:
        MAKE_BUFFER(5, 0) bf_header;
        MAKE_BUFFER(15, 0) bf_payload;

        String url;
        API::Method method;
        HTTPClient http;
        Response* response;

        JsonObject& header;
        JsonObject& payload;
        String host;

        bool OTA_enabled;

        bool parse(JsonObject& data, JsonObject& form);

      public:
        Request(API::Method method, String host, String route = "/");
        ~Request();
        bool addPayload(String key, String value);
        bool addHeader(String key, String value);

        Response* getResponse() { return response; }

        Response* commit();

        bool parse(JsonObject& form);

      };
      Request* req;
      String apiKey;
      String host;
      char* hostname;

  };
}
