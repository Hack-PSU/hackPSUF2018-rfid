#pragma once

#include "component.hpp"

namespace hackPSU{
    class Pollable: public Component{
    public:
        Pollable(char* name) : Component(name) {}
        virtual void poll() = 0;
    };
}