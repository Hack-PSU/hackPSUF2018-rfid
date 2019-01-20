#include "fullbox.h"

using namespace hackPSU;

Box* box;

void setup() {  
  box = new Box("", SSID, PASSWD, DEV, nullptr);
}

void loop() {
  box->cycle();
}
