#include "items.h"
namespace hackPSU{

    //_______________________________________________ Item

    Item::Item(String name, uint32_t id){
        this->name = name;
        this->id = id;
    }
    Item::~Item(){
    }

    //_______________________________________________ Items

    Items::Items(){ 
        current = head = nullptr; 
        length = 0; 
    }

    Items::~Items(){
        while(length-- > 0) {
            Item* tmp = current;
            next();
            delete tmp;
            tmp = nullptr;
        }
    }

    void Items::addItem(Item loc){
        Item* insert = new Item(loc.name, loc.id);

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

    int Items::numItems(){
        return length;
    }

    Item* Items::getCurrent(){
        return current;
    }

    Item* Items::next(){
        current = current->next;
        return current;
    }

    Item* Items::previous(){
        current = current->previous;
        return current;
    }



}