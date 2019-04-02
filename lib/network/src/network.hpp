#pragma once

#include <ArduinoJson.h>
#include <EEPROM.h>

#include <core.hpp>

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
      Network(char* name, String host = ""): Readable<Response*>::Readable(name), Writable<Request*>::Writable(name), Addressable<String>::Addressable() {
        // Addressable method
        setAddress(host);
      }

      Request* createRequest(API::Method method) {
        Request* req = new Request(method, getAddress());
        return req;
      }

      Request* createRequest(API::Method method, String route) {
        Request* req = createRequest(method);
        req->setAddress(route);
        return req;
      }

      Response* sendRequest(Request* request, int retry = 3) {
        if(!request) { return nullptr; }

        pre_send(request);
        write(request);
        
        Response* result = read();
        post_send(request, result);

        return result;
      }


      virtual int status() = 0;
      virtual int connect() = 0;
      virtual String localIP() = 0;
      virtual String mac() = 0;


    protected:
      virtual void pre_send(Request* request) {
        return;
      }
      virtual void post_send(Request* request, Response* response) {
        return;
      }
  };
}
