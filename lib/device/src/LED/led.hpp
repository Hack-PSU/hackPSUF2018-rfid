#include <core.hpp>

#include <Arduino.h>

namespace hackPSU{

    class Led: public Writable<int> {
    public:
        Led(char* name, int pin): Writable<int>::Writable(name), pin(pin) {
            pinMode(pin, OUTPUT);
            digitalWrite(pin, LOW);
        }
        ~Led(){
            Serial.println("LED completed");
        }
    
    protected:
    void transmit(int data){
        if (data > 750) {
            digitalWrite(pin, HIGH);
        }
        else {
            digitalWrite(pin, LOW);
        }
        
    }

    private:
        int pin;
    };

}