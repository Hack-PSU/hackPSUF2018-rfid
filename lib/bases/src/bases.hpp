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

    template<class Packet>
    class Volatile;

    //__________________________________________ Readable
    template<class Packet>
    class Readable: public Component {
    friend class Controller;
    friend class Volatile<Packet>;
    public:
        Readable(char* name):  Component(name) { 
            data = new Packet();
        };
        ~Readable() {}

        virtual Packet read(){
            return scan();
        }

        Packet getData() { 
            return *data; 
        }

    protected:
        volatile Packet* data;
        virtual Packet scan() = 0;  // This method is device specific
        
    };

    class Pollable: public Component{
    public:
        Pollable(char* name) : Component(name) {}
        virtual void poll() = 0;
    };

    //__________________________________________ Volatile
    template<class Packet>
    class Volatile: public Pollable, public Readable<Packet>{
    public:
        Volatile(char* name) : Pollable(name), Readable<Packet>::Readable(name) {}
        void poll(){
            *this->data = this->read();
        }

    };

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


    class Controller{
    public:
        Controller(char* name) {
            name = name;
        }
        virtual void run(){
            while(true){
                cycle();
            }
        }
        virtual void cycle(){
            poll();
            loop();
        }

    private:
        char* name;
        virtual void loop() = 0;
        virtual void poll() = 0;
    };


}