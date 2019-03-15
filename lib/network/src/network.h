#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

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
      GET, POST
    } Method;
  }

  class HTTPCode{
  private:
    int code;
  public:
    HTTPCode(int code): code(code) {}

    String toString() {
      Serial.print("HTTP code: " + String(code));
      switch(code){
        // List of known/common http codes:
        case 200: return F("Success");
        case 401: return F("Unauthorized");
        case 404: return F("Not Found");
        case 409: return F("Wristband Tag already opened");
        case 417: return F("Resource lost");
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
          if(code >= 300) return "Redirection - " + String(code);
          if(code >= 200) return "Success - " + String(code);
          if(code >= 100) return "Informational response - " + String(code);
          return "Unknown code - " + String(code);
      }
    }
    operator int() const{
      return code;
    }
    operator bool() const{
      return code >=200 && code < 300;
    }

  };

  class Response{
  public:
    String payload;
    HTTPCode code;

    Response(String payload, int code): payload(payload), code(code) {}

    operator bool() const{
      return bool(code);
    }

  };

  /**
   * Only one Request per HTTP request
   * */


  class Network{
    public:
      Network(String host);
      void createRequest(API::Method method, String route);
      Response* completeRequest();

      bool connected();
      #if defined(OTA_PASSWORD) && defined(OTA_PASSWORD_HASH)
      void enableOTA();
      void handleOTA();
      #endif


      bool addPayload(String key, String value);
      bool addHeader(String key, String value);
      bool addParameter(String key, String value);

      bool checkApiKey();
      HTTPCode getApiKey(int pin);
      User getDataFromPin(int pin);
      HTTPCode assignUserWID(String wid, int pin);
      User userInfoFromWID(String wid);
      Locations* getEvents();
      User sendScan(String wid, String loc);
      
      Items* getItems();
      HTTPCode itemCheckout(String wid, int iid);
      HTTPCode itemReturn(String wid, int iid);

      Response* commit(bool reboot = true);


    private:
      String apiKey;
      String host;
      bool OTA_enabled;
      char hostname[16];

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
        String parameter;


        bool parse(JsonObject& data, JsonObject& form);

      public:
        Request(API::Method method, String host, String route = "/");
        ~Request();

        bool addPayload(String key, String value);
        bool addHeader(String key, String value);
        bool addParameter(String key, String value);

        Response* getResponse() { return response; }

        Response* commit(bool reboot = true);

        bool parse(JsonObject& form);

      };
      Request* req;
  };
}
