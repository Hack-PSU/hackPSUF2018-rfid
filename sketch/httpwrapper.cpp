#include "httpwrapper.h"

//TODO Get rid of DynamicJsonBuffer for StaticJsonBuffer as it will fragment memory
//https://arduinojson.org/v5/faq/how-to-reduce-memory-usage/
namespace hackPSU {

    HTTPImpl::HTTPImpl(String host) : apiKey(""){
      redisHost = "https://" + host;
    }


    bool HTTPImpl::getAPIKey(){

        //TODO MAKE DAT ONE SECURE BOI
        String url = redisHost+"/auth/scanner/register";
        String payload = "{\"pin\":\""+String(MASTER_KEY)+"\"}";
        int headerCount = 1;
        Headers headers [] = { { "Content-Type", "application/json" } };

        Response* response = HTTP::POST(url, payload, headerCount, headers);

        if (response->responseCode < 0){
            //Free up memory since parsing is complete
            delete response; response = nullptr;
            return nullptr;
        }

        StaticJsonBuffer<500> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(response->payload);

        //Free up memory since parsing is complete
        delete response; response = nullptr;

        //Redis json parse
        String status = root["status"];
        String message = root["message"];  //Should message also be returned to display why user was not allowed in?
        JsonObject& data = root.get<JsonObject>("data");
        apiKey = data.get<String>("apikey");
        //The following is based on assumptions and should be checked
        return (status == "success");
    }

    redisData* HTTPImpl::getDataFromPin(String pin){
        String url = redisHost+"/tabs/getpin";
        String payload = "{\"pin\":"+pin+", \"apikey\": \""+apiKey+"\"}";
        int headerCount = 1;
        Headers headers [] = { { "Content-Type", "application/json" } };


        Response* response = HTTP::POST(url, payload, headerCount, headers);

        if (response->responseCode < 0){
            delete response; response = nullptr;
            return nullptr;
        }

        StaticJsonBuffer<500> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(response->payload);

        //Free up memory since parsing is complete
        delete response; response = nullptr;


        if (String(root.get<char*>("status")) == "error")
          return nullptr;

        //Redis json parse
        redisData* pinData = new redisData;
        JsonObject& data = root.get<JsonObject>("data");
        pinData->uid = data.get<char*>("uid");
        pinData->pin = data.get<char*>("pin");
        pinData->name = data.get<char*>("name");
        pinData->shirtSize = data.get<char*>("shirtSize");
        pinData->diet = data.get<char*>("diet");
        pinData->counter = data.get<char*>("counter");
        pinData->numScans = data.get<char*>("numScans");

        return pinData;
    }

    bool HTTPImpl::assignRfidToUser(String rfidCode, String pin){

        String url = redisHost+"/tabs/setup";
        String payload = "{\"id\":\""+rfidCode+"\", \"pin\":"+pin+", \"apikey\":\""+apiKey+"\"}";
        int headerCount = 1;
        Headers headers [] = { { "Content-Type", "application/json" } };

        Response* response = HTTP::POST(url, payload, headerCount, headers);

        if (response->responseCode < 0){
            //Free up memory since parsing is complete
            delete response; response = nullptr;
            return nullptr;
        }

        StaticJsonBuffer<500> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(response->payload);

        //Free up memory since parsing is complete
        delete response; response = nullptr;

        //Redis json parse
        return root["status"] == "success";
    }

    bool HTTPImpl::entryScan(String locationId, String rfidTag){


        String url = redisHost+"/tabs/add";
        String payload = "{\"location\":\""+locationId+"\" ,\"id\":"+rfidTag+", \"apikey\":\""+apiKey+"\"}";
        int headerCount = 1;
        Headers headers [] = { { "Content-Type", "application/json" } };

        Response* response = HTTP::POST(url, payload, headerCount, headers);

        if (response->responseCode < 0){
            //Free up memory since parsing is complete
            delete response; response = nullptr;
            return nullptr;
        }

        StaticJsonBuffer<500> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(response->payload);

        //Free up memory since parsing is complete
        delete response; response = nullptr;

        //Redis json parse
        String message = root.get<String>("message"); //Should message also be returned to display why user was not allowed in?
        JsonObject& data = root.get<JsonObject>("data");
        bool isRepeat = data.get<bool>("isRepeat");
        //data.get<char*>("name"); use if interested in displaying it down the road
        if (root.get<String>("status") != "success"){
          return nullptr;
        }

        if(isRepeat){
          return nullptr;
        }

        return true;
    }

    Location* HTTPImpl::getLocations(int &len){
        String url = redisHost+"/tabs/active-locations";
        Response* response = HTTP::GET(url);

        if (response->responseCode < 0) {
            delete response; response = nullptr;
            return nullptr;
        }

        DynamicJsonBuffer jsonBuffer(response->payload.length());
        JsonObject& root = jsonBuffer.parseObject(response->payload);
        delete response; response = nullptr;
        len = root["length"]; // TODO: this value
        Location *locations = new Location[len];

        for(int i = 0; i < len; i++){
            locations[i] = {.name = root["locations"][i]["location_name"], .id = root["locations"][i]["uid"]};
        }
        return locations;

    }


}
