#include "locations.h"
namespace hackPSU{

    //_______________________________________________ Location

    Location::Location(String name, uint32_t id){
        this->name = name;
        this->id = id;
        next = previous = nullptr;
    }
    Location::~Location(){
    }

    //_______________________________________________ Locations

    Locations::Locations(){ 
        current = head = nullptr; 
        length = 0; 
    }

    Locations::~Locations(){
        Serial.println("Deleting locations");
        while(current != nullptr){
            Serial.println("Deleting " + current->name);
            Location* tmp = current;
            current->previous->next = current->next;
            current = current->next;
            delete tmp;
        }

    }

    void Locations::addLocation(Location loc){
        Location* insert = new Location(loc.name, loc.id);

        if(length++ == 0) {
            head = current = insert;
            insert->next = insert->previous = insert;
        } else {
            insert->previous = head->previous;
            insert->next = head;

            head->previous->next = insert;
            head->previous = insert;
        }
    }

    int Locations::numLocations(){
        return length;
    }

    Location Locations::getCurrent(){
        return *current;
    }

    Location Locations::next(){
        current = current->next;
        return *current;
    }

    Location Locations::previous(){
        current = current->previous;
        return *current;
    }



}