#pragma once

namespace hackPSU{

    template <class Item>
    class List;

    template <class Item>
    class Wrapper{
        friend class List<Item>;
        Wrapper(Item* item){
            uint16_t size = sizeof(Item);
            data = (Item*) malloc(size);
            memcpy(data, item, size);
        }
        Item* data;
        Wrapper<Item>* previous;
        Wrapper<Item>* next;
    };

    template <class Item>
    class List{
    // Circular Linked list of Items
    public:
        List() {}
        ~List() {}
        void addItem(Item* item) {
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

        void clear() {
            while(length-- > 0) {
                Wrapper<Item>* tmp = current;
                next();
                delete tmp;
                tmp = nullptr;
            }
        }
    private:
        Wrapper<Item>* current;
        Wrapper<Item>* head;
        uint8_t length;
    };
}