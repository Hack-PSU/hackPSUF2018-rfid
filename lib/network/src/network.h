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
  //TODO: move the User and Location structs to a better location where its accessible by all classes
  typedef struct {
    String name;
    String shirtSize;
    String diet;
    int foodCounter;
  } User;

  /*typedef struct {
    String name;
    int id;
  } Location;*/

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
          if(code >= 300) return "Redirection - " + String(code);
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

  class Network{
    public:

      Network(String host);
      Request* createRequest(API::Method method, String route);

      Response* commitRequest();
      Response* commitRequest(JsonObject& form); // include call to parse

      API::Response getApiKey(int pin);
      String getDataFromPin(int pin);
      String assignUserWID(int pin, String wid);
      String userInfoFromWID(String wid);
      Location* getEvents();
    private:
      Request* req;
      String apiKey;
      String host;
  };
}
