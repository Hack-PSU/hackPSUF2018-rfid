#pragma once

#include <Arduino.h>

namespace hackPSU{
    typedef struct {
        String name;
        String shirtSize;
        String diet;
        uint8_t counter;
    } User;

    typedef struct{ 
        String name;
        uint32_t id;
        uint8_t maxEntry;
    } Event;

    typedef struct {
        String name;
        uint32_t id;
    } Item;

    template<class Type>
    struct Pair{
        String key;
        Type* val;
        bool valid;
    };
}