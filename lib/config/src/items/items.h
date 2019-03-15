#pragma once

#include <Arduino.h>

namespace hackPSU{

    class Item{
    friend class Items;
    public:
        Item(String name, uint32_t id);
        ~Item();
        String name;
        uint32_t id;
    private:
        Item* previous;
        Item* next;
    };

    class Items{
    // Circular Linked list of Items
    public:
        Items();
        ~Items();
        void addItem(Item);
        int numItems();
        Item* getCurrent();
        Item* next();
        Item* previous();
    private:
        Item* current;
        Item* head;
        int length;
    };

}