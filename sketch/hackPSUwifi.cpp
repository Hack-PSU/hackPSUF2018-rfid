
//https://techtutorialsx.com/2016/07/21/esp8266-post-requests/
#include "hackPSUwifi.h"

namespace hackPSU {
  String WiFi::errorMessage(int code){
    switch(code){
      // List of known/common http codes:
      case 200: return "Success";
      case 401: return "Unauthorized";
      case 404: return "Not Found";
      case 500: return "Internal Server Error";
      case -1:  return "HTTPC_ERROR_CONNECTION_REFUSED";
      case -2:  return "HTTPC_ERROR_SEND_HEADER_FAILED";
      case -3:  return "HTTPC_ERROR_SEND_PAYLOAD_FAILED";
      case -4:  return "HTTPC_ERROR_NOT_CONNECTED";
      case -5:  return "HTTPC_ERROR_CONNECTION_LOST";
      case -6:  return "HTTPC_ERROR_NO_STREAM";
      case -7:  return "HTTPC_ERROR_NO_HTTP_SERVER";
      case -8:  return "HTTPC_ERROR_TOO_LESS_RAM";
      case -9:  return "HTTPC_ERROR_ENCODING";
      case -10: return "HTTPC_ERROR_STREAM_WRITE";
      case -11: return "HTTPC_ERROR_READ_TIMEOUT";
      default:
        if(code >= 500) return "Server error - " + String(code);
        if(code >= 400) return "Client error - " + String(code);
        if(code >= 300) return "Redirectio - n" + String(code);
        if(code >= 200) return "Success - " + String(code);
        if(code >= 100) return "Informational response - " + String(code);
        return "Unknown code - " + String(code);
    }
  }

  void WiFi::request(Methods_t method, String url){
    
    // Begin HTTP request
    #if HTTPS
      http.begin(HOST, PORT, url, HTTPS, FP);
    #else
      http.begin(HOST, PORT, url, HTTPS, "");
    #endif
    
    // Set headers, if any, for request
    for(JsonPair& p: headers){
      http.addHeader(p.key, p.value.as<char*>());
    }

    String pld = "";
    payload.printTo(pld);

    // Reset JSON buffer and corresponding objects
    bf_payload.clear();
    payload = bf_payload.createObject();

    bf_header.clear();
    headers = bf_header.createObject();
    
    // Send appropriate request and save the data
    Response* response = new Response;
    
    if(method == GET)       response->code = http.get();
    else if(method == POST) responlocationsse->code = http.post(pld);

    response->payload = http.getString();

    // Terminate HTTP request
    http.end();

    // Return response
    return response;
  }

  WiFi::WiFi(){
    WiFi.begin(ssid, password);

    payload = bf_payload.createObject();
    headers = bf_headers.createObject();
 
  }

  void WiFi::parse(String payload, JsonObject& output){

    #ifdef STATIC
      StaticJsonBuffer<500> buffer;
    #else defined(DYNAMIC)
      DynamicJsonBuffer buffer;
    #endif
    JsonObject& data = buffer.parseObject(payload);

    parse(data, output);
  }
  
  void WiFi::parse(JsonObject& payload, JsonObject& output){
    for(JsonPair& p: output){ 
      if(p.value.is<JsonObject>()) {
        parse(payload[p.key], p.value);
      } else if(p.value.is<JsonArray>()) {
          // TODO: handle array filtering
          p.value = payload[p.key];
      } else {
        p.value = payload[p.key]; // Let caller use .as<T>()
      }
    }
  }

  API::Response WiFi::getApiKey() {
    payload.set("pin",String(MASTER_KEY));
    headers.set("Content-Type", "application/json");
    Response* registerScanner;
    registerScanner = request(POST, "/auth/scanner/register");

    API::Response res;
    if(pinData->code == 200){
      #if defined(STATIC)
      StaticJsonBuffer<JSON_OBJECT_SIZE(5)> jsonBuffer;
      #elif defined(DYNAMIC)
      DynamicJsonBuffer jsonBuffer;
      #endif
      JsonObject& data = jsonBuffer.parseObject(registerScanner->payload);
      apiKey = data.get<String>("apikey");

      res = SUCCESS;
    } else {
      res = parseCode(pinData->code);
    }

    delete registerScanner;
    return res;
  }

  API::Response WiFi::getDataFromPin(String pin, JsonObject& output) {

    payload.set("pin", pin);
    payload.set("apiKey", apiKey);

    headers.set("Content-Type", "application/json");

    Response* pinData = request(POST, "/tabs/getpin");
    
    API::Response res;
    if(pinData->code == 200){
      parse(pinData->payload, output);
      res = SUCCESS;
    } else {
      res = parseCode(pinData->code);
    }

    delete pinData;
    return res;

  }

  API::Response WiFi::assignRfidToUser(String rfidCode, String pin, JsonObject& output){
    /**
     * pass/fail
     */
    payload.set("id", rfidCode);
    payload.set("pin", pin);
    payload.set("apikey", apiKey);

    headers.set("Content-Type", "application/json");

    Response* rfidData = request(POST, "/tabs/setup");

    API::Response res;
    if(rfidData->code == 200) {
      parse(rfidData->payload, output);
      res = SUCCESS;
    } else {
      res = parseCode(rfidData->code);
    }

    delete rfidData;
    return res;
  }

  API::Response WiFi::entryScan(String locationID, String rfidCode, JsonObject& output){
    /**
     * isRepeat
     * status
     */
    payload.set("id", rfidCode);
    payload.set("location", locationID);
    payload.set("apikey", apiKey);

    headers.set("Content-Type", "application/json");

    Response* scanidData = request(POST, "/tabs/add");

    API::Response res;
    if(scanData->code == 200) {
      parse(scanData->payload, output);
      res = SUCCESS;
    } else {
      res = parseCode(scanData->code);
    }

    delete scanData;
    return res;
  }  

  API::Response WiFi::getLocations(String rfidCode, String pin, JsonObject& output){
    payload.set("id", rfidCode);
    payload.set("pin", pin);
    payload.set("apikey", apiKey);

    headers.set("Content-Type", "application/json");

    Response* locationData = request(POST, "/tabs/setup");

    API::Response res;
    if(locationData->code == 200) {
      parse(locationData->payload, output);
      res = SUCCESS;
    } else {
      res = parseCode(locationData->code);
    }

    delete locationData;
    return res;
  }

  API::Response WiFi::parseCode(int code){
    switch(code){
    case 401:
    case 404:
    case 409: 
        return FAIL;
    case 200: 
        return SUCCESS;
    case 500: 
        return TIMEOUT;
    default:
        return REDIS_DOWN;
}
  }
}
