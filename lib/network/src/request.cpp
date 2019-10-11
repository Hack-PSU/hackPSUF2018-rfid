#include "request.hpp"

namespace hackPSU{

  Request::Request(API::Method method, String route) :
    Addressable<String>::Addressable(),
    header(bf_header.createObject()),
    payload(bf_payload.createObject())
  {
      this->method = method;
      this->host = REDIS;
      setAddress(route);
  }

  bool Request::addPayload(String key, String value){
    return payload.set(key, value);
  }
  bool Request::addHeader(String key, String value){
    return header.set(key, value);
  }

  bool Request::addParameter(String key, String value){
    parameter += (parameter.length() == 0 ? '?' : '&') + key + "=" + value;
    return true;
  }

  String Request::generateURL(String host){
    #ifdef HTTPS_FINGERPRINT
      String url = "https://" + host + this->getAddress() + parameter;
    #else
      String url = "http://" + host + this->getAddress() + parameter;
    #endif
    #ifdef DEBUG
      Serial.print("Using URL: ");
      Serial.println(url);
    #endif
    return url;
  }

  bool Request::isMethod(API::Method method){
    return this->method == method;
  }


  JsonObject& Request::getHeader() {
    return header;
  }
  JsonObject& Request::getPayload() {
    return payload;
  }

}