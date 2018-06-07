#include "hackPSUrfid.h"
#include "hackPSUstatemachine.h"
using namespace hackPSU;

Scanner scanner(0,0);
StateMachine fsm();

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println(scanner.getUID());
}

void chageState(){
  
}

