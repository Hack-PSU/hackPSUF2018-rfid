#pragma once

#include "component.hpp"

namespace hackPSU
{
    
    template<class Packet>
    class Volatile;

    //__________________________________________ Readable
    template<class Packet>
    class Readable: public Component {
    friend class Controller;
    friend class Volatile<Packet>;
    public:
        Readable(char* name):  
            Component(name)
        { 
            data = new Packet();
        };
        ~Readable() {
            delete data;
        }

        virtual Packet read(){
            return scan();
        }

        Packet getData() { 
            return *data; 
        }

    protected:
        Packet* data;
        virtual Packet scan() = 0;  // This method is device specific
        
    };
} // hackPSU
