#pragma once

#include "component.hpp"


namespace hackPSU{
    //__________________________________________ Writable 
    template<class Packet>
    class Writable: public Component {
    public:
        Writable(char* name):  Component(name) { };
        ~Writable() {}
        virtual void write(Packet* data, uint32_t length = 0){
            transmit(data, length);
        }
    protected:
        virtual void transmit(Packet*, uint32_t) = 0;
    };
}