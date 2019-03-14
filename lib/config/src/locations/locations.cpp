#include "locations.h"
namespace hackPSU{

    //_______________________________________________ Location

    Location::Location(String name, uint32_t id){
        this->name = name;
        this->id = id;
    }
    Location::~Location(){
    }

    //_______________________________________________ Locations

    Locations::Locations(){ 
        current = head = nullptr; 
        length = 0; 
    }

    Locations::~Locations(){
        while(length-- > 0) {
        //for(int i = 0; i < length && current != nullptr; ++i){
            Serial.println("Deleting Location: " + current->name);
            Location* tmp = current;
            next();
            delete tmp;
            tmp = nullptr;
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
        Serial.println("Successfully added location: "+ loc.name);
        Serial.println("Current locatin: " + current->name);
    }

    int Locations::numLocations(){
        return length;
    }

    Location* Locations::getCurrent(){
        return current;
    }

    Location* Locations::next(){
        current = current->next;
        return current;
    }

    Location* Locations::previous(){
        current = current->previous;
        return current;
    }



}