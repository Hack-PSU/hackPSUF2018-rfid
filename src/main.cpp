#include <hackpsu/fullbox/fullbox.h>
 
hackPSU::Box *scanner;

void setup() {
  delay(2000);
  Serial.begin(9600); 

  // Box(String redis_addr, const char* ssid, const char* password, Mode_e mode, const byte* band_key=nullptr);
  scanner = new hackPSU::Box(REDIS, NETWORK_SSID, NETWORK_PASSWORD, hackPSU::DEV);

}

void loop() {
  scanner->cycle();
}
