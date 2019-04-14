#pragma once

#include <stdint.h>

namespace hackPSU {
    template <class Item>
    class List;

    template <class Item>
    class Wrapper{
        friend class List<Item>;
        Wrapper(Item* item){
            data = item;
            previous = next = this;
        }
        ~Wrapper() {
            delete data;
            data = nullptr;
        }
        Item* data;
        Wrapper<Item>* previous;
        Wrapper<Item>* next;
    };

    template <class Item>
    class List{
    // Circular Linked list of Items
    public:
        List() {
            length = 0;
        }
        ~List() {
            clear();
        }
        void addItem(Item* item) {

            Wrapper<Item>* insert = new Wrapper<Item>(item);

            if(length++ == 0) {
                head = current = insert;
            } else {
                insert->previous = head->previous;
                insert->next = head;

                head->previous->next = insert;
                head->previous = insert;
            }
        }

        uint8_t size() {
            return length;
        }

        inline Item* getCurrent() {
            return current->data;
        }

        Item* next() {
            current = current->next;
            return getCurrent();
        }

        Item* previous() {
            current = current->previous;
            return getCurrent();
        }

        Item* reset() {
            current = head;
            return getCurrent();
        }

        void clear() {
            while(length > 0) {
                deleteCurrent();
            }
            head = nullptr;
            current = nullptr;
        }

    private:
        Wrapper<Item>* current;
        Wrapper<Item>* head;
        uint8_t length;

        void deleteCurrent() {

            if(length > 0) {

                Wrapper<Item>* tmp = current;
                tmp->next->previous = tmp->previous;
                current = tmp->previous->next = tmp->next;
                delete tmp;
                tmp = nullptr;
                --length;   
            }
        }
    };
}