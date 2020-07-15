#pragma once

#include "network.hpp"
#include "storage.hpp"
#include "objects.h"
#include "codes.h"

#include <list.hpp>

#ifndef API_VERSION
#error Macro, API_VERSION, not set
#define API_VERSION ""
#endif

// [-1,-100] are reserved codes for device-specific error codes
#define API_PARSE_ERROR -100
#define API_FIELD_MISSING -101
#define API_INVALID_VALUE -102
#define API_NOT_IMPLIMENTED -103
#define API_INVALID_KEY -104

namespace hackPSU{

    enum Fields { WID, PIN };

    class Api : public Network{
    public:
        Api(char* name);
        Api(char* name, String host);

        virtual ~Api() = default;

        /**
         * checkApiKey
         * 
         * Description:
         *   Checks if the stored API key is valid
         * 
         * Input:
         * 
         * Output:
         * - http return code
         *   * HTTP_PARSE_ERROR: unable to read response
         *   * 
         */
        int checkApiKey();

        /**
         * getApiKey
         * 
         * Description:
         *   Fetches a new API key from server, stores it in memory
         * 
         * Input:
         * - pin: Hacker pin
         * 
         * Output:
         * - http return code
         */
        int getApiKey(String pin);

        /**
         * getUserInfo
         * 
         * Description:
         *   Fetches user data given a pin or wid
         * 
         * Input:
         * - field: indicates which field to get user info from 
         * - key: value for given field
         * - user: User object to store retrieved data into
         * 
         * Output:
         * - http return code
         */
        int getUserInfo(Fields field, String key, User* user);

        /**
         * registerUser
         * 
         * Description:
         *   Associates a user to a wristband
         * 
         * Input:
         * - wid: wristband id
         * - pin: Hacker pin,
         * - data: User object to store retrieved data into
         * 
         * Output:
         * - http return code
         */
        int registerUser(String wid, String pin);

        /**
         * getEvents
         * 
         * Description:
         *   fetches a list of all active events
         * 
         * Input:
         * - list: A list to insert all events into
         * 
         * Output:
         * - http return code
         */
        int getEvents(List<Event>* list);

        /**
         * sendScan
         * 
         * Description:
         *   Sends event scan to server
         * 
         * Input:
         * - wid: wristband id
         * - eid: event id
         * - data: User object to store retrieved data into
         * 
         * Output:
         * - http return code
         */
        int sendScan(String wid, Event* event, User* user, uint32_t offset);

        int getItems(List<Item>* list);
        int itemCheckout(String wid, Item* item);
        int itemReturn(String wid, Item* item);

        virtual void enableUpdate() = 0;
        virtual void handleUpdate() = 0;

        virtual String decode(int code);

    protected:
        void pre_send(Request* request) override;
        void post_send(Request* request, Response* response) override;

    private:
        String apiKey; // API key require for authentication
        uint32_t start;   // Offset used for scan time


        /**
         * storeApiKey
         * 
         * Description:
         *   Stores the API key in memory for use after power cycle
         * 
         * Input:
         * - apiKey (member variable must be set)
         * Output:
         * 
         */
        void storeApiKey();


        /**
         * loadApiKey
         * 
         * Description:
         *   Loads the API key from memory for use after power cycle
         * 
         * Input:
         * 
         * Output:
         * - apiKey (member variable) will be set
         * 
         */
        void loadApiKey();

        /**
         * extract
         * 
         * Descrition:
         *   Extracts a list series of KV pairs
         * 
         * Input:
         * - json: the JsonObject which would have the desired KV pairs
         * - keys: a Pair object which has the key, pointer to start the result at, and a valid flag
         * 
         * Output:
         * - true if all pairs were extracted successfully
         */
        template<class Type>
        bool extract(JsonObject& json, Pair<Type> keys[]);

        /**
         * extract
         * 
         * Descrition:
         *   Extracts data of a given key
         * 
         * Input:
         * - json: the JsonObject which would have the desired KV pairs
         * - key: the key to try extracting a value of
         * - data: pointer of where to store the result
         * 
         * Output:
         * - true if data was extracted successfully
         */
        template<class Type>
        bool extract(JsonObject& json, Type& data, String key = "");

        bool extract(JsonObject& json, User* data);
        bool extract(JsonObject& json, Item* data);
        bool extract(JsonObject& json, Event* data);

        int cleanup(Request* req, Response* res);
    };
}