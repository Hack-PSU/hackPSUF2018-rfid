#pragma once

#include "api.hpp"

namespace hackPSU{

    inline int freeAndReturn(Response* res){
        int tmp = res->code;
        delete res;
        return tmp;
    }

    Api::Api(char* name):
        Network(name)
    {
        // Get API key from memory
        EEPROM.begin(37);
        loadApiKey();

        #if defined(DEBUG) && defined(SERIAL_EN)
        Serial.println("API key: " + apiKey);
        #endif

        start = 0;
    }

    Api::Api(char* name, String host):
        Api(name)
    {
        setAddress(host);
    }

    void Api::loadApiKey(){
        char apibuff[37];
        EEPROM.get(0, apibuff);
        apiKey = String(apibuff);
    }
    
    void Api::storeApiKey(){
        char apibuff[37];
        apiKey.toCharArray(apibuff, 37);
        EEPROM.put(0, apibuff);
        EEPROM.commit();
    }

    int Api::checkApiKey() {
        Request* req = createRequest(API::POST, "/auth/scanner/time");
        req->addParameter("apiKey", apiKey);

        Response* res = sendRequest(req, 3);

        if(bool(*res) || res->code == 401){
            MAKE_BUFFER(25, 25) bf_data;
            JsonObject& response = bf_data.parseObject(res->payload);
            if(response.success()){
                if(res->code != 401 && extract<String>(response, &apiKey, "apiKey")) {
                    storeApiKey();
                }

                if (! (extract<uint32_t>(response, &start, "time")) ){
                    res->code = API_FIELD_MISSING;
                }
            } else {
                res->code = API_PARSE_ERROR;
            }
        }
        return freeAndReturn(res);
    }

    int Api::getApiKey(String pin) {

        Request* req = createRequest(API::POST, "/auth/scanner/register");
        req->addPayload("pin",String(pin));

        Response* res = sendRequest(req, 3);

        if(bool(*res)){
            MAKE_BUFFER(25, 25) bf_data;
            JsonObject& response = bf_data.parseObject(res->payload);

            if(response.success()) {
                if(extract<String>(response.get<JsonObject>("data"), &apiKey, "apiKey")) {
                    storeApiKey();
                }
            } else {
                res->code = API_PARSE_ERROR;
            }
        }
        return freeAndReturn(res);
    }

    int Api::getUserInfo(Fields field, String id, User* user){
        Request* req = createRequest(API::POST);

        switch(field){
        case PIN:
            req->setAddress("/rfid/getpin");
            req->addPayload("pin", id);
            break;
        case WID:
            req->setAddress("/rfid/user-info");
            req->addPayload("wid", id);
            break;
        default:
            return API_INVALID_VALUE;
        }

        Response* res = sendRequest(req);
        if(bool(*res)){
            MAKE_BUFFER(25, 25) bf_data;
            JsonObject& response = bf_data.parseObject(res->payload);

            if(response.success()){
                JsonObject& data = response.get<JsonObject>("data");
                if(! (extract(data, user)) ) {
                    res->code = API_FIELD_MISSING;
                }
            } else {
                res->code = API_PARSE_ERROR;
            }
        }
        return freeAndReturn(res);
    }

    int Api::registerUser(String wid, String pin) {
        Request* req = createRequest(API::POST, "/rfid/assign");
        req->addPayload("pin", String(pin));
        req->addPayload("wid", wid);

        Response* res = sendRequest(req);

        return freeAndReturn(res);
    }

    int Api::getEvents(List<Event>* list) {
        Request* req = createRequest(API::GET, "/rfid/items");

        Response* res = sendRequest(req);

        if((*res)){
            MAKE_BUFFER(25, 25) bf_data;
            JsonObject& response = bf_data.parseObject(res->payload);

            if(response.success()){
                JsonArray& events = response.get<JsonArray>("items");

                for(JsonObject& event : events){
                    Event* e = new Event();
                    if(extract(event, e)) {
                        list->addItem(e);
                    }
                }
            } else {
                res->code = API_PARSE_ERROR;
            }
        }
        return freeAndReturn(res);
    }

    int Api::sendScan(String wid, Event* event, User* user, uint32_t offset) {
        Request* req = createRequest(API::POST, "/rfid/scan");
        req->addPayload("wid", wid);
        req->addPayload("location", String(event->id));
        req->addPayload("timestamp", String(start + offset));

        Response* res = sendRequest(req);
        if(bool(*res)){
            MAKE_BUFFER(25, 25) bf_data;
            JsonObject& response = bf_data.parseObject(res->payload);

            if(response.success()){
                JsonObject& data = response.get<JsonObject>("data");
                if(! (extract(data, user)) ) {
                    res->code = API_FIELD_MISSING;
                }
            } else {
                res->code = API_PARSE_ERROR;
            }
        }
        return freeAndReturn(res);
    }

    void Api::pre_send(Request* request){
        request->addHeader("Content-Type", "application/json");
        request->addHeader("macaddr", mac());
        if(request->isMethod(API::GET)){
            request->addParameter("version", API_VERSION);
            request->addParameter("apikey", apiKey);
        } else {
            request->addPayload("version", API_VERSION);
            request->addPayload("apikey", apiKey);
        }
    }

    template<class Type>
    bool Api::extract(JsonObject& json, Type* data, String key){
        if(json.containsKey(key)) {
            *data = json.get<Type>(key);
            return true;
        } else {
            return false;
        }
    }

    bool Api::extract(JsonObject& json, User* user){
        return this->extract<String>(json, &user->name, "name") && 
               this->extract<String>(json, &user->shirtSize, "shirtSize") && 
               this->extract<String>(json, &user->diet, "diet");
    }

    bool Api::extract(JsonObject& json, Event* event){
        return this->extract<String>(json, &event->name, "name") && 
               this->extract<uint32_t>(json, &event->id, "id") && 
               this->extract<uint8_t>(json, &event->maxEntry, "maxEntry");
    }

    bool Api::extract(JsonObject& json, Item* item){
        return this->extract<String>(json, &item->name, "name") && 
               this->extract<uint32_t>(json, &item->id, "id");
    }

    template<class Type>
    bool Api::extract(JsonObject& json, Pair<Type> keys[]){
        bool success = true;
        for(Pair<Type> p : keys) {
            p.valid = extract<Type>(json, p.val, p.key);
            if(!p.valid){
                success = false;
            }
        }
        return success;
    }

}