
#include "hackPSUstatemachine.h"

using namespace hackPSU;

Box* box;

void setup() {  
  Serial.begin(9600);
  box = new Box(REDIS, SSID, PASSWD, DEV, nullptr);
}

void loop() {
  box->cycle();
}
