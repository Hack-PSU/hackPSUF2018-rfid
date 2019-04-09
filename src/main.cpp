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
    if(input != term) pin += input;
  } while(input != term);

  return pin;
}

void printUsage() {
  Serial.println("Usage: " + String(100 - double(ESP.getFreeHeap()) / 819.20) + "%");
}

void setup() {
  Serial.begin(9600); 
  delay(2000);
  esp = new hackPSU::ESP8266_Device((char*)"esp");


  while(esp->status() != WL_CONNECTED){
    Serial.println("Not conected: " + String(esp->status()));
    delay(500);
  }
  
  //_______________________________________________ Checking API key
  Serial.println("Verifying current API key");
  int result = esp->checkApiKey();
  if(result == OK) {
    Serial.println("API key good!");
  } else {
    Serial.println("Need to update API key: " + esp->decode(result));
    //_______________________________________________ Update API key

    Serial.println("Fetching new API key...");
    Serial.print("Pin: ");

    String pin = getPin();
    Serial.println(esp->decode(esp->getApiKey(pin)));
  }



  //_______________________________________________ Fetch events

  Serial.println("Fetching list of events...");
  hackPSU::List<hackPSU::Event>* events = new hackPSU::List<hackPSU::Event>();
  Serial.println("Fetch code: " + esp->decode(esp->getEvents(events)));
  Serial.println("There are " + String(events->size()) + " events");

  hackPSU::Event* event = nullptr;

  if(events->size() > 0) {
    event = events->getCurrent();
    Serial.println("Current event: " + event->name);
  }

  //_______________________________________________ Fetch items

  Serial.println("Fetching list of items...");
  hackPSU::List<hackPSU::Item>* items = new hackPSU::List<hackPSU::Item>();
  Serial.println("Fetch code: " + esp->decode(esp->getItems(items)));
  Serial.println("There are " + String(items->size()) + " items");

  if(items->size() > 0)
    Serial.println("Current items: " + items->getCurrent()->name);  

  
  //_______________________________________________ Get User info
  
  Serial.print("Enter a pin to fetch user data: ");
  String userPin = getPin();

  hackPSU::User* user = new hackPSU::User();

  
  Serial.println("Fetch code: " + esp->decode(esp->getUserInfo(hackPSU::PIN, userPin, user)));
  Serial.println("Data for " + user->name);
  Serial.println("Shirt Size: " + user->shirtSize);
  Serial.println("Diet:       " + user->diet);
  Serial.println("Counter:    " + String(user->counter));
  
  //_______________________________________________ Registering user
  
  Serial.println("Registering " + user->name);
  String wid = "123456789";
  Serial.println("Code: " + esp->decode(esp->registerUser(wid, userPin)));

  //_______________________________________________ Scanning user into evetn
  if(events->size() > 0) {
    Serial.println("Scanning " + user->name + " into " + event->name);
    Serial.println("Code: " + esp->decode(esp->sendScan(wid, event, user, millis())));
  } else {
    Serial.println("No events to scan in for...");
  }

  // Box(String redis_addr, const char* ssid, const char* password, Mode_e mode, const byte* band_key=nullptr);
  // scanner = new hackPSU::Box(REDIS, NETWORK_SSID, NETWORK_PASSWORD, hackPSU::DEV);

}

void loop() {
  // scanner->cycle();
}
