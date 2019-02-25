#pragma once

namespace hackPSU{

    //__________________________________________ Component
    class Component {
    public:
        Component(char* name) : name(name) {}
        ~Component() {}
        char* getName();

    protected:

    private:
        char* name;
    };
}