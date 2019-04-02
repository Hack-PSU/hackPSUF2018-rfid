#include "list.hpp"

namespace hackPSU{

    //_______________________________________________ Location

    template <class Item>
    List<Item>::List(){
    }

    template <class Item>
    List<Item>::~List(){
        clear();
    }
    
    template <class Item>
    void List<Item>::addItem(Item* item) {
        Wrapper<Item>* insert = new Wrapper<Item>(item);

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

    template <class Item>
    uint8_t List<Item>::size() {
        return length;
    }

    template <class Item>
    Item* List<Item>::getCurrent() {
        return current;
    }

    template <class Item>
    Item* List<Item>::next() {
        current = current->next;
        return current;
    }

    template <class Item>
    Item* List<Item>::previous() {
        current = current->previous;
        return current;
    }

    template <class Item>
    void List<Item>::clear() {
        while(length-- > 0) {
            Wrapper<Item>* tmp = current;
            next();
            delete tmp;
            tmp = nullptr;
        }
    }

}