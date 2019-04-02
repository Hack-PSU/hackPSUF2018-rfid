#pragma once

#include <ArduinoJson.h>
#include <EEPROM.h>

#include <addressable.hpp>

#include "response.hpp"

namespace hackPSU{


    namespace API{
        typedef enum {
            GET, POST
        } Method;
    }

    #if defined(DYNAMIC_BUFFER)
        //#warning Using dynamic buffer size for JSON data
        #define MAKE_BUFFER(obj_size, arr_size) DynamicJsonBuffer
    #else
        #define MAKE_BUFFER(obj_size, arr_size) StaticJsonBuffer<JSON_OBJECT_SIZE(obj_size)+JSON_ARRAY_SIZE(arr_size)>
    #endif

    class Request: public Addressable<String>{
    friend class Api;
    public:
        Request(API::Method method, String host);
        ~Request();

        bool addPayload(String key, String value);
        bool addHeader(String key, String value);
        bool addParameter(String key, String value);

        String generateURL(String host);

        bool isMethod(API::Method method);

        JsonObject& getHeader();
        JsonObject& getPayload();

    private:
        MAKE_BUFFER(5, 0) bf_header;
        MAKE_BUFFER(15, 0) bf_payload;

        API::Method method;
        Response* response;

        JsonObject& header;
        JsonObject& payload;
        String host;
        String parameter;
    };
}