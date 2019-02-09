#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "bases.hpp"

namespace hackPSU {
        
    class STDOUT : public Writable<char*>{
    public: 
        STDOUT(char* name) : Writable<char*>::Writable(name) {}

    protected:
        void transmit(char* p){
            std::cout << p;
        }

    };

    class STDIN: public Volatile<char*>{
    public: 
        STDIN(char* name) : Volatile<char*>::Volatile(name) {}

    protected:
        char* scan(){
            if(rand() % 2){
               return nullptr; 
            }
            char* c = (char*)malloc(sizeof(char) * 2);
            c[0] = rand() % ('z' - 'a') + 'a';
            c[1] = '\0';
            return c;
        }
    };

    class test_ctrl: public Controller{
    public:
        test_ctrl(char* name) : Controller(name) {
            char name_input[]="input";
            char name_output[]="output";
            input = new STDIN(name_input);
            output = new STDOUT(name_output);
        }

    protected:

    private:
        STDIN* input;
        STDOUT* output;

        void loop(){
            char* read = input->getData();

            if( read ) {
                output->write(read);
            }
        }

        void poll(){
            input->poll();
        }

    };

}


int main(){
    char name_controller[] = "controller";
    hackPSU::test_ctrl controller(name_controller);

    std::cout << "Generating 25 values\n";
    for(int i = 0; i < 25; ++i) controller.cycle();
    std::cout << "\nDone!\n";
    return 0;
}