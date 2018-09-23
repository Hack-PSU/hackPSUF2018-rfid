
#include "hackPSUstatemachine.h"
#include <ESP8266WiFi.h>

using namespace hackPSU;

Box* box;

void setup() {  
  box = new Box(REDIS, SSID, PASSWD, DEV, nullptr);
}

void loop() {
  box->cycle();
}
