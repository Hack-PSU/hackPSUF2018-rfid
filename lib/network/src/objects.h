#pragma once

#include <Arduino.h>

namespace hackPSU{
    struct User {
        String name;
        String shirtSize;
        String diet;
        uint8_t counter;
    };

    struct Event { 
        String name;
        String id;
        uint8_t maxEntry;
    } ;

    struct Item {
        String name;
        uint32_t id;
    };

    template<class Type>
    struct Pair{
        String key;
        Type* val;
        bool valid;
    };
}