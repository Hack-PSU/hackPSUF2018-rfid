#pragma once

#include "component.hpp"


namespace hackPSU{
    //__________________________________________ Writable 
    template<class Packet>
    class Writable: public Component {
    public:
        Writable(char* name):  Component(name) { };
        ~Writable() {}
        virtual void write(Packet data){
            transmit(data);
        }
    protected:
        virtual void transmit(Packet) = 0;
    };
}