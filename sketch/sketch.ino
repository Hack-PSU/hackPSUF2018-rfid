
#include "hackPSUstatemachine.h"

using namespace hackPSU;

Box* box;

#define REDIS  "c-98-235-90-5.hsd1.pa.comcast.net"
#define SSID   "psuwirelesssetup"
#define PASSWD ""

void setup() {  
  box = new Box(REDIS, SSID, PASSWD, DEV, nullptr);
}

void loop() {
  box->cycle();
}
