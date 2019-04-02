#pragma once


namespace hackPSU{
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