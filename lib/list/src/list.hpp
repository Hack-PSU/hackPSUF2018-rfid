#pragma once

#include <Arduino.h>

namespace hackPSU{

    template <class Item>
    class Wrapper{
        Wrapper(Item* item){
            uint16_t size = sizeof(Item);
            data = (Item*) malloc(size);
            memcpy(data, item, size);
        }
        Item* data;
        Item* previous;
        Item* next;
    };

    template <class Item>
    class List{
    // Circular Linked list of Items
    public:
        List();
        ~List();
        void addItem(Item*);
        uint8_t size();
        Item* getCurrent();
        Item* next();
        Item* previous();
        void clear();
    private:
        Wrapper<Item>* current;
        Wrapper<Item>* head;
        uint8_t length;
    };
}