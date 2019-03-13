
//https://techtutorialsx.com/2016/07/21/esp8266-post-requests/
#include "network.h"

namespace hackPSU {

  //___________________________________________________________________ Request
  Request::Request(API::Method method, String host, String route) :
      header(bf_header.createObject()),
      payload(bf_payload.createObject()),
      method(method)
  {
    #ifdef DEBUG
      Serial.println(host + route);
    #endif
    // Set the base URL for the request
    #ifdef HTTPS
      url = "https://" + host + route;
    #else
      url = "http://" + host + route;
    #endif
    #ifdef DEBUG
      Serial.print("THE URL is: ");
      Serial.println(url);
    #endif
    addHeader("Content-Type", "application/json");
    addHeader("macaddr", WiFi.macAddress());
    addPayload("version", API_VERSION);

    response = nullptr;
  }

  Request::~Request(){
    if(response != nullptr) delete response;
  }

  bool Request::addPayload(String key, String value){
    return payload.set(key, value);
  }
  bool Request::addHeader(String key, String value){
    return header.set(key, value);
  }

  Response* Request::commit() {
    String builtURL = url;
    //Add payload to url if GET method
    if(method == API::GET){
      builtURL += "?";
      for(JsonPair& p: payload){
        builtURL += p.key;
        builtURL += "=";
        builtURL += p.value.as<char*>();
        builtURL += "&";
      }
      builtURL.remove(builtURL.length()-1);
    }
    // Begin HTTP request
    #ifdef HTTPS
      //http.begin(HOST, PORT, url, true, FP);
      http.begin(builtURL, FP);
    #else
      http.begin(builtURL);
    #endif
    // Set headers, if any, for request
    for(JsonPair& p: header){
      http.addHeader(p.key, p.value.as<char*>());
    }

    int code;
    if(method == API::GET) {
      code = http.GET();
    }
    else if(method == API::POST) {
      String pld = "";
      payload.printTo(pld);
      code = http.POST(pld);
    }
    response = new Response(http.getString(),code);
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
    //apiKey = "805bc162-7d21-40cc-bc9b-1e0722c3ff88";
    #ifdef DEBUG
      Serial.print("The host is: ");
      Serial.println(host);
      Serial.println(NETWORK_SSID);
      Serial.println(NETWORK_PASSWORD);
    #endif
    WiFi.begin(NETWORK_SSID, NETWORK_PASSWORD);
  }

  Request* Network::createRequest(API::Method method, String route){
    if(req != nullptr) delete req;
    req = nullptr;
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

  HTTPCode Network::getApiKey(int pin) {
    Request* req = createRequest(API::POST, "/auth/scanner/register");
    req -> payload.set("pin",String(pin));
    req -> payload.set("version", API_VERSION);

    Response* registerScanner = req->commit();


    if(bool(*registerScanner)){
      MAKE_BUFFER(25, 25) bf_data;
      JsonObject& response = bf_data.parseObject(registerScanner->payload);

      JsonObject& data = response.get<JsonObject>("data");
      apiKey = data.get<String>("apikey");
      #ifdef DEBUG
        Serial.println(apiKey);
      #endif

    }
    return registerScanner->code;
  }

  User Network::getDataFromPin(int pin) {
    Request* req = createRequest(API::POST, "/rfid/getpin");
    req -> payload.set("pin", String(pin));
    req -> payload.set("version", API_VERSION);
    req -> payload.set("apikey", apiKey);
    Response* registerScanner = req->commit();
    User user = {.name = "NULL", .shirtSize = "NULL", .diet = "NULL", .allow = false };
    if(bool(*registerScanner)){
      MAKE_BUFFER(25, 25) bf_data;
      JsonObject& response = bf_data.parseObject(registerScanner->payload);
      JsonObject& data = response.get<JsonObject>("data");
      user.name = data.get<String>("name");
      user.shirtSize = data.get<String>("shirtSize");
      user.diet = data.get<String>("diet");
      user.allow = true;

    }
    #ifdef DEBUG
      Serial.print("Response Code: ");
      Serial.println(String(registerScanner->code));
    #endif
    return user;
  }

  HTTPCode Network::assignUserWID(int pin, String wid) {
    Request* req = createRequest(API::POST, "/rfid/assign");
    req -> payload.set("pin", String(pin));
    req -> payload.set("wid", wid);
    req -> payload.set("apikey", apiKey);
    Response* registerScanner = req->commit();
    #ifdef DEBUG
      Serial.print("Response Code: ");
      Serial.println(String(registerScanner->code));
    #endif
    return registerScanner->code;
  }

  User Network::userInfoFromWID(String wid) {
    Request* req = createRequest(API::GET, "/rfid/user-info");
    req -> payload.set("wid", wid);
    req -> payload.set("apikey", apiKey);
    Response* registerScanner = req->commit();
    User user = {.name = "NULL", .shirtSize = "NULL", .diet = "NULL", .allow = false };
    if(*registerScanner){
      MAKE_BUFFER(25, 25) bf_data;
      JsonObject& response = bf_data.parseObject(registerScanner->payload);
      JsonObject& data = response.get<JsonObject>("data");
      user.name = data.get<String>("name");
      user.shirtSize = data.get<String>("shirtSize");
      user.diet = data.get<String>("diet");
      user.allow = true;

    }
    #ifdef DEBUG
      Serial.print("Response Code: ");
      Serial.println(String(registerScanner->code));
    #endif
    return user;
  }

  Locations Network::getEvents() {
    Request* req = createRequest(API::GET, "/rfid/events");
    Response* registerScanner = req->commit();
    if(*registerScanner){
      MAKE_BUFFER(25, 25) bf_data;
      JsonObject& response = bf_data.parseObject(registerScanner->payload);
      int length = response.get<int>("length");
      JsonArray& jsonLoc = response.get<JsonArray>("locations");
      Location *locations = new Location[length];
      for(int i = 0; i < length; i++){
        locations[i] = {.name = jsonLoc[i]["event_title"], .id = jsonLoc[i]["event_location"]};
      }
      //TODO: clean this up
      #ifdef DEBUG
        Serial.print("Response Code: ");
        Serial.println(String(registerScanner->code));
      #endif
      Locations loc = {.data = locations, .length = length};
      return loc;

    }

    #ifdef DEBUG
      Serial.print("Response Code: ");
      Serial.println(String(registerScanner->code));
    #endif
    Locations loc = {.data = nullptr, .length = 0};
    return loc;
  }

  User Network::sendScan(String wid, int loc) {
    Request* req = createRequest(API::POST, "/rfid/scan");
    req -> payload.set("wid", wid);
    req -> payload.set("location", String(loc));
    req -> payload.set("apikey", apiKey);
    Response* registerScanner = req->commit();
    User user = {.name = "NULL", .shirtSize = "NULL", .diet = "NULL", .allow = false };
    if(*registerScanner){
      MAKE_BUFFER(25, 25) bf_data;
      JsonObject& response = bf_data.parseObject(registerScanner->payload);
      JsonObject& data = response.get<JsonObject>("data");

      user.name = data.get<String>("name");
      user.shirtSize = data.get<String>("shirtSize");
      user.diet = data.get<String>("diet");
      user.allow = !(data.get<bool>("isRepeat"));

    }

    #ifdef DEBUG
      Serial.print("Response Code: ");
      Serial.println(String(registerScanner->code));
    #endif
    return user;
  }

}
