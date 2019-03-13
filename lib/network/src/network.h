#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include <config.h>

// Preform checks for used macros
#ifndef API_VERSION
#error API verision not specified
#elif !defined(MASTER_KEY)
#error Master key not specified
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
   * This class handles specific requests
   * */
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

    virtual Response* commit();

    bool parse(JsonObject& form);

  };

  class Network{
    public:

      Network(String host);
      Request* createRequest(API::Method method, String route);

      Response* commitRequest();
      Response* commitRequest(JsonObject& form); // include call to parse

      HTTPCode getApiKey(int pin);
      User getDataFromPin(int pin);
      HTTPCode assignUserWID(int pin, String wid);
      User userInfoFromWID(String wid);
      Locations getEvents();
      User sendScan(String wid, int loc);
    private:
      Request* req;
      String apiKey;
      String host;
  };
}
