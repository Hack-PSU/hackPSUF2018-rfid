#pragma once

#include <ArduinoJson.h>
#include <EEPROM.h>

#include <readable.hpp>
#include <writable.hpp>
#include <addressable.hpp>

#include "response.hpp"
#include "request.hpp"

// Preform checks for used macros

#ifndef MASTER_KEY
#error Macro, MASTER_KEY, not set
#define MASTER_KEY ""
#endif

#ifndef REDIS
#error Macro, REDIS, not set
#define REDIS ""
#endif

#ifndef RETRY_RATE
#define RETRY_RATE 3
#endif



namespace hackPSU {

  #ifdef HTTPS_FINGERPRINT
    constexpr uint8_t FP[20] = { HTTPS_FINGERPRINT };
  #endif


  class Network: public Readable<Response*>, public Writable<Request*>, public Addressable<String> {
    public:
      Network(char* name, String host);
      Network(char* name);

      Request* createRequest(API::Method method);
      Request* createRequest(API::Method method, String route);
      Response* sendRequest(Request* request, int retry = 3);


      virtual int status() = 0;
      virtual int connect() = 0;
      virtual String localIP() = 0;
      virtual String mac() = 0;


    private:
      virtual void pre_send(Request* request) = 0;
      virtual void post_send(Request* request, Response* response) = 0;
  };
}
