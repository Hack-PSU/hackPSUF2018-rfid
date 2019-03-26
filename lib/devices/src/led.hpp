#include <core.hpp>

#include <Arduino.h>

namespace hackPSU{

    class led: public Writable<int> {
    public:
        led(char* name, int pin1): Writable<int>::Writable(name), pin(pin1) {
            pinMode(pin, OUTPUT);
            digitalWrite(pin, LOW);
        }
        ~led(){
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