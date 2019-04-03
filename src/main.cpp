// #include "hackpsu/fullbox.h"
 
// hackPSU::Box *scanner;

#include <esp8266/esp8266.hpp>
#include <list.hpp>

hackPSU::ESP8266_Device* esp;

String getPin(char term = '\n') {
  char input = '\0';
  String pin = "";
  do {
    while(!Serial.available());
    input = Serial.read();
    Serial.print(input);
    pin += input;
  } while(input != term);

  return pin;
}

void printUsage() {
  Serial.println("Usage: " + String(100 - double(ESP.getFreeHeap()) / 819.20) + "%");
}

void setup() {
  Serial.begin(9600); 
  delay(2000);
  printUsage();
  esp = new hackPSU::ESP8266_Device((char*)"esp");
  printUsage();


  while(esp->status() != WL_CONNECTED){
    Serial.println("Not conected: " + String(esp->status()));
    delay(500);
  }

  /*
    Serial.println("Fetching list of events...");
    printUsage();
    hackPSU::List<hackPSU::Event>* events = new hackPSU::List<hackPSU::Event>();
    printUsage();
    Serial.println("Fetch code: " + esp->decode(esp->getEvents(events)));
    printUsage();
    Serial.println("There are " + String(events->size()) + " events");

    hackPSU::Event *e = new hackPSU::Event();
    e->id = "3b697e8dc49045f3a9dc6d76a3d80ba5";
    e->maxEntry = 1;
    e->name = "Dinner";

    events->addItem(e);

    if(events->size() > 0)
      Serial.println("Current event: " + events->getCurrent()->name);

  /*/
    Serial.println("Fetching list of items...");
    printUsage();
    hackPSU::List<hackPSU::Item>* items = new hackPSU::List<hackPSU::Item>();
    printUsage();
    Serial.println("Fetch code: " + esp->decode(esp->getItems(items)));
    printUsage();
    Serial.println("There are " + String(items->size()) + " items");

    if(items->size() > 0)
      Serial.println("Current items: " + items->getCurrent()->name);
  //*/

  
  printUsage();




  // Box(String redis_addr, const char* ssid, const char* password, Mode_e mode, const byte* band_key=nullptr);
  // scanner = new hackPSU::Box(REDIS, NETWORK_SSID, NETWORK_PASSWORD, hackPSU::DEV);

}

void loop() {
  // scanner->cycle();
}
