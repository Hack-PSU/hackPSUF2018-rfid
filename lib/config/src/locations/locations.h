#pragma once

#include <Arduino.h>

namespace hackPSU{

    class Location{
    friend class Locations;
    public:
        Location(String name, uint32_t id);
        ~Location();
        String name;
        uint32_t id;
    private:
        Location* previous;
        Location* next;
    };

    class Locations{
    // Circular Linked list of locations
    public:
        Locations();
        ~Locations();
        void addLocation(Location);
        int numLocations();
        Location getCurrent();
        Location next();
        Location previous();
    private:
        Location* current;
        Location* head;
        int length;
    };

}