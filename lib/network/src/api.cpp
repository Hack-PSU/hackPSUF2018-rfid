#include "api.hpp"

namespace hackPSU{


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

    Api::Api(char* name, String host): Api(name) {
        setAddress(host);
    }

    void Api::loadApiKey(){
        char apibuff[37];
        EEPROM.get(0, apibuff);
        apibuff[36] = '\0';
        apiKey = String(apibuff);
    }
    
    void Api::storeApiKey(){
        char apibuff[37];
        apiKey.toCharArray(apibuff, 37);
        apibuff[36] = '\0';
        EEPROM.put(0, apibuff);
        EEPROM.commit();
    }

    int Api::checkApiKey() {
        if( !isAlphaNumeric(apiKey[0]) ) {
            return 401;
        }
        
        Request* req = createRequest(API::POST,  "/auth/scanner/verify");

        Response* res = sendRequest(req, 3);

        if(bool(*res) || res->code == 401){
            MAKE_BUFFER(25, 25) bf_data;
            JsonObject& response = bf_data.parseObject(res->payload);

            if(response.success()){
                JsonObject& data = response.get<JsonObject>("data");
                bool valid;
                if(!extract<bool>(data, valid, "isValid")) {
                    res->code = API_PARSE_ERROR;
                } else if(!valid) {
                    res->code = API_INVALID_KEY;
                }
                if (! (extract<uint32_t>(data, start, "time")) ){
                    res->code = API_FIELD_MISSING;
                }
            } else {
                res->code = API_PARSE_ERROR;
            }
        }

        return cleanup(req, res);
    }

    int Api::getApiKey(String pin) {
        Request* req = createRequest(API::POST, "/auth/scanner/register");
        req->addPayload("pin",String(pin));

        Response* res = sendRequest(req, 3);

        if(bool(*res)){
            MAKE_BUFFER(25, 25) bf_data;
            JsonObject& response = bf_data.parseObject(res->payload);

            if(response.success()) {
                JsonObject& data = response.get<JsonObject>("data");

                if(extract<String>(data, apiKey, "apikey")) {
                    storeApiKey();
                } else {
                    res->code = API_FIELD_MISSING;
                }
            } else {
                res->code = API_PARSE_ERROR;
            }
        }
        return cleanup(req, res);
    }

    int Api::getUserInfo(Fields field, String id, User* user){
        Request* req = createRequest(API::POST);

        switch(field){
        case PIN:
            req->setAddress("/scanner/getpin");
            req->addPayload("pin", id);
            break;
        case WID:
            req->setAddress("/scanner/user-info");
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

        return cleanup(req, res);
    }

    int Api::registerUser(String wid, String pin) {
        Request* req = createRequest(API::POST, "/scanner/assign");
        req->addPayload("pin", String(pin));
        req->addPayload("wid", wid);

        Response* res = sendRequest(req);

        return cleanup(req, res);
    }

    int Api::getEvents(List<Event>* list) {
        Request* req = createRequest(API::GET, "/scanner/events");

        Response* res = sendRequest(req);

        if(bool(*res)){
            MAKE_BUFFER(25, 25) bf_data;
            JsonObject& response = bf_data.parseObject(res->payload);
            if(response.success()){

                if(response.get<int>("length") > 0) {
                    JsonArray& events = response.get<JsonArray>("locations");

                    for(JsonObject& event : events){
                        Event* e = new Event();
                        if(extract(event, e)) {
                            Serial.println("Adding event: " + e->name);
                            list->addItem(e);
                        } else {
                            res->code = API_INVALID_VALUE;
                        }
                    }
                }
            } else {
                res->code = API_PARSE_ERROR;
            }
        }

        return cleanup(req, res);
    }

    int Api::sendScan(String wid, Event* event, User* user, uint32_t offset) {
        Request* req = createRequest(API::POST, "/scanner/scan");
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

        return cleanup(req, res);
    }

    int Api::getItems(List<Item>* list) {
        Request* req = createRequest(API::GET, "/scanner/items");

        Response* res = sendRequest(req);

        if((*res)){
            MAKE_BUFFER(25, 25) bf_data;
            JsonObject& response = bf_data.parseObject(res->payload);

            if(response.success()){
                if(response.get<int>("length") > 0) {
                    JsonArray& items = response.get<JsonArray>("items");

                    for(JsonObject& event : items){
                        Item* item = new Item();
                        if(extract(event, item)) {
                            list->addItem(item);
                        } else {
                            res->code = API_FIELD_MISSING;
                        }
                    }
                }
            } else {
                res->code = API_PARSE_ERROR;
            }
        }

        return cleanup(req, res);
    }

    int Api::itemCheckout(String wid, Item* item) {
        #warning Route not implemented
        return API_NOT_IMPLIMENTED;
        // Request* req = createRequest(API::POST, "/scanner/checkout");
        // req->addPayload("wid", wid);
        // req->addPayload("itemId", String(item->id));

        // Response* res = sendRequest(req);

        // return cleanup(req, res);
    }

    int Api::itemReturn(String wid, Item* item) {
        #warning Route not implemented
        return API_NOT_IMPLIMENTED;
        // Request* req = createRequest(API::POST, "/scanner/return");
        // req->addPayload("wid", wid);
        // req->addPayload("itemId", String(item->id));

        // Response* res = sendRequest(req);

        // return cleanup(req, res);
    }


    void Api::pre_send(Request* request){
        request->addHeader("Content-Type", "application/json");
        request->addHeader("macaddr", mac());

        if(request->isMethod(API::GET)){
            request->addParameter("version", API_VERSION);
            request->addParameter("apikey", apiKey+"\0");  // make sure API key is null terminated
        } else {
            request->addPayload("version", API_VERSION);
            request->addPayload("apikey", apiKey+"\0");
        }
    }

    void Api::post_send(Request* request, Response* response) {
        #if defined(DEBUG) && defined(SERIAL_EN)
        Serial.println(response->payload);
        #endif
    }

    int Api::cleanup(Request* req, Response* res){
        int tmp = res->code;

        delete req;
        delete res;

        return tmp;
    }

    template<class Type>
    bool Api::extract(JsonObject& json, Type& data, String key){
        if(json.containsKey(key)) {
            data = json.get<Type>(key);
            return true;
        } else {
            #if defined(DEBUG) && defined(SERIAL_EN)
            Serial.println("Could not extract data for key, " + key);
            #endif
            return false;
        }
    }

    bool Api::extract(JsonObject& json, User* user){
        #warning Update fields to extract for type "User"
        return this->extract<String>(json, user->name, "name") && 
               this->extract<String>(json, user->shirtSize, "shirtSize") && 
               this->extract<String>(json, user->diet, "diet") &&
               this->extract<uint8_t>(json, user->counter, "counter");
    }

    bool Api::extract(JsonObject& json, Event* event){
        #warning Hard coding event max entry value
        event->maxEntry = 1;
        return this->extract<String>(json, event->name, "event_title") && 
               this->extract<String>(json, event->id, "uid") /* && 
               this->extract<uint8_t>(json, event->maxEntry, "maxEntry") */;
    }

    bool Api::extract(JsonObject& json, Item* item){
        return this->extract<String>(json, item->name, "name") && 
               this->extract<String>(json, item->id, "uid");
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

    String Api::decode(int code) {
        switch(code) {
            case API_FIELD_MISSING:     return F("Missing field");
            case API_INVALID_VALUE:     return F("Invalid value");
            case API_PARSE_ERROR:       return F("Parse Error");
            case API_NOT_IMPLIMENTED:   return F("Route not implemented");
            case API_INVALID_KEY:       return F("Invalid API key");
            case OUT_OF_MEMORY:         return F("Out of memory");
            case OK:                    return F("Success");
            case UNAUTHORIZED:          return F("Unauthorized");
            case NOT_FOUND:             return F("Entity not found");
            case INTERNAL_SERVER_ERROR: return F("Server error");


            default:                return String(code);
        }
    }
}