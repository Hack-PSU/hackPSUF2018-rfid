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

    int tmpcode;
    if(method == API::GET) {
      tmpcode = http.GET();
    } else if(method == API::POST) {
      String pld = "";
      payload.printTo(pld);
      tmpcode = http.POST(pld);
    }
    response = new Response(http.getString(), tmpcode);
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

    // Get API key from memory
    char apibuff[36];
    EEPROM.begin(36);
    EEPROM.get(0, apibuff);
    apiKey = String(apibuff);
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

    // TODO: add api key check here

    createRequest(API::POST, "/auth/scanner/register");
    addPayload("pin",String(pin));
    addPayload("version", API_VERSION);

    Response* registerScanner = commit();

    if(bool(*registerScanner)){
      MAKE_BUFFER(25, 25) bf_data;
      JsonObject& response = bf_data.parseObject(registerScanner->payload);

      JsonObject& data = response.get<JsonObject>("data");
      apiKey = data.get<String>("apikey");
      Serial.println(apiKey);
      char apibuff[36];
      apiKey.toCharArray(apibuff, 37);
      EEPROM.put(0, apibuff);
      EEPROM.commit();
      Serial.println(apibuff);
      #ifdef DEBUG
        EEPROM.get(0, apibuff);
        Serial.print("APIBUFF (getpin): ");
        Serial.println(apibuff);
      #endif

    } else {
      Serial.println("Failed updating API key");
      Serial.println(registerScanner->code.toString());
    }
    Serial.println("Using apikey: " + apiKey);
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
      Serial.println(registerScanner->code.toString());
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
      Serial.println(registerScanner->code.toString());
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
      Serial.println(registerScanner->code.toString());
    #endif
    return user;
  }


  Response* Network::commit(){
    return req->commit();
  }

  Locations* Network::getEvents() {
    createRequest(API::GET, "/rfid/events");
    Response* registerScanner = commit();
    Locations* list = new Locations();
    if(*registerScanner){
      Serial.println("Received list");
      MAKE_BUFFER(25, 25) bf_data;
      JsonObject& response = bf_data.parseObject(registerScanner->payload);
      int length = response.get<int>("length");
      //Serial.println("Found "+String(length)+" events");
      JsonArray& jsonLoc = response.get<JsonArray>("locations");

      for(int i = 0; i < length; i++){
        //Serial.println("Adding evetn: " + String(jsonLoc[i]["event_title"].asString()));
        Location tmp(jsonLoc[i]["event_title"], jsonLoc[i]["event_location"]);
        list->addLocation(tmp);
      }
    } else {
      Serial.println("Failed");
    }
    #ifdef DEBUG
      Location tmp("testing", 9);
      list->addLocation(tmp);
      Serial.println("Current item: " + list->getCurrent()->name);
      Serial.print("Response Code: ");
      Serial.println(registerScanner->code.toString());
    #endif
    return list;
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
      Serial.println(registerScanner->code.toString());
    #endif
    return user;
  }

}
