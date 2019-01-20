
//https://techtutorialsx.com/2016/07/21/esp8266-post-requests/
#include "network.h"

namespace hackPSU {

  //___________________________________________________________________ Request
  Request::Request(API::Method method, String host, String route) : 
      header(bf_header.createObject()), 
      payload(bf_payload.createObject()),
      method(method)
  {
    // Set the base URL for the request
    #ifdef HTTPS
      url = "https://" + host + route;
    #else
      url = "http://"  + host + route;
    #endif

    addHeader("Content-Type", "application/json");
    // addHeader("mac", WiFi.macAddress());
    addPayload("version", API_VERSION);

    response = new Response();
  }

  Request::~Request(){
    delete response;
  }

  bool Request::addPayload(String key, String value){
    return payload.set(key, value);
  }
  bool Request::addHeader(String key, String value){
    return header.set(key, value);
  }

  Response* Request::commit() {
    // Begin HTTP request
    #ifdef HTTPS
      //http.begin(HOST, PORT, url, true, FP);
      http.begin(url, FP);
    #else
      http.begin(url);
    #endif
    
    // Set headers, if any, for request
    for(JsonPair& p: header){
      http.addHeader(p.key, p.value.as<char*>());
    }

    String pld = "";
    payload.printTo(pld);
    
    if(method == API::GET)       response->code = http.GET();
    else if(method == API::POST) response->code = http.POST(pld);

    response->payload = http.getString();

    // Terminate HTTP request
    http.end();

    return response;
  }

  bool Request::parse(JsonObject& data, JsonObject& form){
    /*
    for (const JsonPair& pair : form) {
      if(data.containsKey(pair.key)){
        Serial.print("Key, " + String(pair.key));
        if(data.is<JsonObject>(pair.key)){
          form.createNestedObject(pair.key);
          if( !parse(data.get<JsonObject>(pair.key), form.get<JsonObject>(pair.key)) ) { return false; }
          
        } else if(data.is<JsonArray>(pair.key)){
          // TODO: deep copy of form[pair.key]
          form.createNestedArray(pair.key);
          int count = 0;
          for(JsonObject& item : data[pair.key]){
            // [ {'a':'A'}, {'b':'B'}]
          }

          // Decide how to handle this
          // JsonArray.add(value)
          ;
        } else if(data.is<char*>(pair.key)){
          if(!form.set(pair.key, data.get<String>(pair.key))) { return false; };
        } else if(data.is<int>(pair.key)){
          if(!form.set(pair.key, data.get<int>(pair.key))) { return false; };
        } else if(data.is<float>(pair.key)){
          if(!form.set(pair.key, data.get<float>(pair.key))) { return false; };
        } else if(data.is<bool>(pair.key)){
          if(!form.set(pair.key, data.get<bool>(pair.key))) { return false; };
        } else {
          Serial.println(", is an unknown type" + data.get<String>(pair.key));
          return false;
        }
      }
    }
    //*/
    return true;
  }

  //___________________________________________________________________ Network

  Network::Network(String host): host(host) {
    req = nullptr;
    WiFi.begin(SSID, PASSWD);
  }

  Request* Network::createRequest(API::Method method, String route){
    if(req != nullptr) delete req;
    req = new Request(method, host, route);
    return req;
  }

  Response* Network::commitRequest(){
    if( req == nullptr) return nullptr;

    return req->commit();
  }

  Response* Network::commitRequest(JsonObject& form){
    if( req == nullptr) return nullptr;


    Response* res = req->commit();

    MAKE_BUFFER(50, 50) bf_result;
    JsonObject& result = bf_result.parseObject(res->payload);

    req->parse(form);
    return res;
  }

  API::Response Network::getApiKey() {
    Request* req = new Request(API::POST, "/auth/scanner/register");
    req -> payload.set("pin",String(MASTER_KEY));
    req -> payload.set("version", API_VERSION);

    Response* registerScanner = req->commit();

    API::Response res;
    if(registerScanner->code == 200){
      MAKE_BUFFER(25, 25) bf_data;
      JsonObject& data = bf_data.parseObject(registerScanner->payload);


      apiKey = data.get<String>("apikey");

      res = data.get<String>("version") == API_VERSION ? API::SUCCESS : API::OUTDATED;

    } else {
      res = static_cast<API::Response>(registerScanner->code);
    }

    delete registerScanner;
    return res;
  }
}
