#pragma once

#include "pollable.hpp"
#include "readable.hpp"

namespace hackPSU
{
    //__________________________________________ Volatile
    template<class Packet>
    class Volatile: public Pollable, public Readable<Packet>{
    public:
        Volatile(char* name) : Pollable(name), Readable<Packet>::Readable(name) {}
        void poll(){
            *this->data = this->read();
        }

    };
} // hackPSU
