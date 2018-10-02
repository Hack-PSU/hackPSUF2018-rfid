#include "httpwrapper.h"
#include <cstring>

//TODO Get rid of DynamicJsonBuffer for StaticJsonBuffer as it will fragment memory
//https://arduinojson.org/v5/faq/how-to-reduce-memory-usage/
namespace hackPSU {

    // DO NOT USE IDEALLY
    RedisData::RedisData() {
        uid = "";
        pin = "";
        uid = "";
        pin = "";
        name = "";
        shirtSize = "";
        diet = "";
        counter = "";
        numScans = "";
    }

    RedisData::RedisData(
        const char *uid,
        const char *pin,
        const char *name,
        const char *shirtSize,
        const char *diet,
        const char *counter,
        const char *numScans
    ) {
        if (uid != nullptr) {
            this->uid = new char [strlen(uid)];
            strcpy(this->uid, uid);
        } else {
            this->uid = "";
        }
        if (pin != nullptr) {
            this->pin = new char [strlen(pin)];
            strcpy(this->pin, pin);
        } else {
            this->pin = "";
        }
        if (name != nullptr) {
            this->name = new char [strlen(name)];
            strcpy(this->name, name);
        } else {
            this->name = "";
        }
        if (shirtSize != nullptr) {
            this->shirtSize = new char [strlen(shirtSize)];
            strcpy(this->shirtSize, shirtSize);
        } else {
            this->shirtSize = "";
        }
        if (diet != nullptr) {
            this->diet = new char [strlen(diet)];
            strcpy(this->diet, diet);
        } else {
            this->diet = "";
        }
        if (counter != nullptr) {
            this->counter = new char [strlen(counter)];
            strcpy(this->counter, counter);
        } else {
            this->counter = "";
        }
        if (numScans != nullptr) {
            this->numScans = new char [strlen(numScans)];
            strcpy(this->numScans, numScans);
        } else {
            this->numScans = "";
        }
    }

    RedisData::~RedisData() {
        delete uid;
        delete pin;
        delete name;
        delete shirtSize;
        delete diet;
        delete counter;
        delete numScans;
    }


    HTTPImpl::HTTPImpl(String host) : apiKey(""){
      redisHost = "https://" + host;
    }
    
    api_response HTTPImpl::getAPIKey(){

        String url = redisHost+"/auth/scanner/register";
        String payload = "{\"pin\":\""+String(MASTER_KEY)+"\"}";
        int headerCount = 1;
        Headers headers [] = { { "Content-Type", "application/json" } };

        Response* response = HTTP::POST(url, payload, headerCount, headers);


        switch(response->responseCode){
            case 401:
                delete response;  
                return FAIL;
            case 200: 
                break;
            case 500: 
                delete response;  
                return TIMEOUT;
            default:
                delete response;  
                return REDIS_DOWN;
        }

        if (response->responseCode < 0){
            //Free up memory since parsing is complete
            delete response;
            return nullptr;
        }

        StaticJsonBuffer<500> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(response->payload);

        //Free up memory since parsing is complete
        delete response; // response = nullptr;

        //Redis json parse
        String status = root["status"];
        String message = root["message"];  //Should message also be returned to display why user was not allowed in?
        JsonObject& data = root.get<JsonObject>("data");
        apiKey = data.get<String>("apikey");

        //The following is based on assumptions and should be checked
        return (status == "success")? SUCCESS: FAIL;
    }

    RedisData* HTTPImpl::getDataFromPin(String pin){
        String url = redisHost+"/tabs/getpin";
        String payload = "{\"pin\":"+pin+", \"apikey\": \""+apiKey+"\"}";
        int headerCount = 1;
        Headers headers [] = { { "Content-Type", "application/json" } };


        Response* response = HTTP::POST(url, payload, headerCount, headers);

        if (response->responseCode != 200){
            delete response;
            response = nullptr;
            return nullptr;
        }
        Serial.println(response->payload);
        StaticJsonBuffer<500> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(response->payload);

        //Free up memory since parsing is complete
        delete response; 
        response = nullptr;


        if (String(root.get<char*>("status")) == "error") {
          return nullptr;
        }

        // Redis json parse
        JsonObject& data = root.get<JsonObject>("data");
        RedisData* pinData = new RedisData(
            data.get<char*>("uid"),
            data.get<char*>("pin"),
            data.get<char*>("name"),
            data.get<char*>("shirtSize"),
            data.get<char*>("diet"),
            data.get<char*>("counter"),
            data.get<char*>("numScans")
        );

        return pinData;
    }

    api_response HTTPImpl::assignRfidToUser(String rfidCode, String pin){

        String url = redisHost+"/tabs/setup";
        String payload = "{\"id\":\""+rfidCode+"\", \"pin\":"+pin+", \"apikey\":\""+apiKey+"\"}";
        int headerCount = 1;
        Headers headers [] = { { "Content-Type", "application/json" } };

        Response* response = HTTP::POST(url, payload, headerCount, headers);

        //404 means pin already registered
        //409 means uid is reused (scrap wrist band)
        //switch case for these with default as call handle error and print 
    

        // StaticJsonBuffer<500> jsonBuffer;
        // JsonObject& root = jsonBuffer.parseObject(response->payload);

        switch(response->responseCode){
            case 401:
            case 404:
            case 409:
                delete response;  
                return FAIL;
            case 200: 
                break;
            case 500: 
                delete response;  
                return TIMEOUT;
            default:
                delete response;  
                return REDIS_DOWN;
        }

        if (response->responseCode < 0){
            //Free up memory since parsing is complete
            delete response;
            return FAIL;
        }

        //Free up memory since parsing is complete
        delete response;// response = nullptr;

        //Redis json parse
        return SUCCESS;
    }

    bool HTTPImpl::entryScan(String locationId, String rfidTag){


        String url = redisHost+"/tabs/add";
        String payload = "{\"location\":\""+locationId+"\" ,\"id\":"+rfidTag+", \"apikey\":\""+apiKey+"\"}";
        int headerCount = 1;
        Headers headers [] = { { "Content-Type", "application/json" } };

        Response* response = HTTP::POST(url, payload, headerCount, headers);

        if (response->responseCode != 200){
            //Free up memory since parsing is complete
            delete response;
            return false;
        }

        StaticJsonBuffer<500> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(response->payload);

        //Free up memory since parsing is complete
        delete response; 
        response = nullptr;

        //Redis json parse
        // String message = root.get<String>("message"); // Use if needed
        JsonObject& data = root.get<JsonObject>("data");
        bool isRepeat = data.get<bool>("isRepeat");
        //data.get<char*>("name"); use if interested in displaying it down the road
        if (strncmp(root.get<char *>("status"), "success", 7)){
          return false;
        }

        return !isRepeat;
    }

    Location* HTTPImpl::getLocations(int &len){
        String url = redisHost+"/tabs/active-locations";
        Response* response = HTTP::GET(url);

        if (response->responseCode != 200) {
            delete response;
            return nullptr;
        }

        DynamicJsonBuffer jsonBuffer(response->payload.length());
        JsonObject& root = jsonBuffer.parseObject(response->payload);
        delete response;// response = nullptr;
        len = root["length"]; // TODO: this value
        Location *locations = new Location[len];

        for(int i = 0; i < len; i++){
            locations[i] = {.name = root["locations"][i]["location_name"], .id = root["locations"][i]["uid"]};
        }
        return locations;
    }
}
