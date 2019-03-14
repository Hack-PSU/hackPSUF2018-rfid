#include "network.h"

namespace hackPSU {

  //___________________________________________________________________ Request
  Network::Request::Request(API::Method method, String host, String route) :
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

  Network::Request::~Request(){
    if(response != nullptr) delete response;
  }

  bool Network::Request::addPayload(String key, String value){
    return payload.set(key, value);
  }
  bool Network::Request::addHeader(String key, String value){
    return header.set(key, value);
  }

  bool Network::Request::addParameter(String key, String value){
    parameter += (parameter.length() == 0 ? '?' : '&') + key + "=" + value;
    return true;
  }

  Response* Network::Request::commit() {
    // Begin HTTP request
    #ifdef HTTPS
      //http.begin(HOST, PORT, url, true, FP);
      http.begin(url + parameter, FP);
    #else
      http.begin(url + parameter);
    #endif
    
    // Set headers, if any, for request
    for(JsonPair& p: header){
      http.addHeader(p.key, p.value.as<char*>());
    }

    int code;
    if(method == API::GET) {
      code = http.GET();
    } else if(method == API::POST) {
      String pld = "";
      payload.printTo(pld);
      code = http.POST(pld);
    }
    response = new Response(http.getString(),code);
    // Terminate HTTP request
    http.end();
    return response;
  }

  bool Network::addHeader(String key, String value){
    return req->addHeader(key, value);
  }
  bool Network::addPayload(String key, String value){
    return req->addPayload(key, value);
  }
  bool Network::addParameter(String key, String value){
    return req->addParameter(key, value);
  }

  //___________________________________________________________________ Network

  Network::Network(String host) {
    this->host = host;
    OTA_enabled = false;
    req = nullptr;
    WiFi.begin(NETWORK_SSID, NETWORK_PASSWORD);
    String h = "hackpsu_scanner";
    h.toCharArray(hostname, 16);
  }


  void Network::createRequest(API::Method method, String route){
    if(req != nullptr) delete req;
    req = nullptr;
    req = new Request(method, host, route);
  }

  Response* Network::completeRequest(){
    if( req == nullptr) return nullptr;

    return commit();
  }

  bool Network::connected(){
    return WiFi.status() == WL_CONNECTED;
  }

  HTTPCode Network::getApiKey(int pin) {
    createRequest(API::POST, "/auth/scanner/register");
    addPayload("pin",String(pin));
    addPayload("version", API_VERSION);

    Response* registerScanner = commit();


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
    createRequest(API::POST, "/rfid/getpin");
    addPayload("pin", String(pin));
    addPayload("version", API_VERSION);
    addPayload("apikey", apiKey);
    Response* registerScanner = commit();
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
    createRequest(API::POST, "/rfid/assign");
    addPayload("pin", String(pin));
    addPayload("wid", wid);
    addPayload("apikey", apiKey);
    Response* registerScanner = commit();
    #ifdef DEBUG
      Serial.print("Response Code: ");
      Serial.println(String(registerScanner->code));
    #endif
    return registerScanner->code;
  }

  #if defined(OTA_PASSWORD) && defined(OTA_PASSWORD_HASH)
  void Network::enableOTA(){
    if(OTA_enabled) { return; }

    ArduinoOTA.setPort(8266);
    ArduinoOTA.setHostname(hostname);
    ArduinoOTA.setPassword((char*)OTA_PASSWORD);
    ArduinoOTA.setPasswordHash((char*)OTA_PASSWORD_HASH); // hd5(password)
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
        } else { // U_SPIFFS
        type = "filesystem";
        }

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
          Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
          Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
          Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
          Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
          Serial.println("End Failed");
        }
    });

    ArduinoOTA.begin();
    OTA_enabled = true;

  }
  void Network::handleOTA(){
    ArduinoOTA.handle();
  }
  #endif
  User Network::userInfoFromWID(String wid) {
    createRequest(API::GET, "/rfid/user-info");
    addParameter("wid", wid);
    addParameter("apikey", apiKey);
    Response* registerScanner = commit();
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


  Response* Network::commit(){
    return req->commit();
  }

  Locations Network::getEvents() {
    createRequest(API::GET, "/rfid/events");
    Response* registerScanner = commit();
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
    createRequest(API::POST, "/rfid/scan");
    addPayload("wid", wid);
    addPayload("location", String(loc));
    addPayload("apikey", apiKey);
    Response* registerScanner = commit();
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
