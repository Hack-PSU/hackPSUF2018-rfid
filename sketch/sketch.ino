
#include "hackPSUstatemachine.h"

using namespace hackPSU;

Box* box;

#define REDIS  "127.0.0.1"
#define SSID   "psuwirelesssetup"
#define PASSWD ""

void setup() {  
  box = new Box(REDIS, SSID, PASSWD, DEV, nullptr);
}

void loop() {
  box->cycle();
}
