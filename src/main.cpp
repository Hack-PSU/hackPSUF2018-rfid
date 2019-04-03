// #include "hackpsu/fullbox.h"
 
// hackPSU::Box *scanner;

#include <esp8266/esp8266.hpp>
#include <list.hpp>

hackPSU::ESP8266_Device* esp;

void setup() {
  Serial.begin(9600); 
  esp = new hackPSU::ESP8266_Device((char*)"esp");
  delay(2000);


  while(esp->status() != WL_CONNECTED){
    Serial.println("Not conected: " + String(esp->status()));
    delay(500);
  }

  // Serial.print("Enter API key pin: ");

  // while(!Serial.available());

  // String pin = Serial.readString();
  // Serial.println(pin);
  
  // Serial.println("Fetching API key...");
  // Serial.println("Fetch code: " + String(esp->getApiKey(pin)));

  Serial.println("Fetching list of items...");
  hackPSU::List<hackPSU::Item>* items = new hackPSU::List<hackPSU::Item>();
  Serial.println("Fetch code: " + esp->decode(esp->getItems(items)));
  Serial.println("There are " + String(items->size()) + " items");

  if(items->size() > 0)
    Serial.println("Current item: " + items->getCurrent()->name);




  // Box(String redis_addr, const char* ssid, const char* password, Mode_e mode, const byte* band_key=nullptr);
  // scanner = new hackPSU::Box(REDIS, NETWORK_SSID, NETWORK_PASSWORD, hackPSU::DEV);

}

void loop() {
  // scanner->cycle();
}
