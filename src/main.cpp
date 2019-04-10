#include "hackpsu/fullbox.h"
 
hackPSU::Box *scanner;

void setup() {
  scanner = new hackPSU::Box(REDIS, NETWORK_SSID, NETWORK_PASSWORD, hackPSU::DEV);

}

void loop() {
  scanner->cycle();
}
