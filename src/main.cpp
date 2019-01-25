#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <network.h>

#ifndef REDIS
#error Macro, REDIS, not set
#endif

//*
hackPSU::Network* http;


void setup(){
  delay(1000);

  Serial.begin(9600);


  http = new hackPSU::Network(String(REDIS));
  Serial.println("WiFi connecting");
  while(WiFi.status() != WL_CONNECTED) 
    yield();
  Serial.printf("WiFi connected to %s\n", "matthew");
  if(http->getApiKey() == hackPSU::API::SUCCESS){
    Serial.println("Successful acquiring API key");
  } else {
    Serial.println("Failed acquiring API key");
  }
}
void loop(){
  ;
}
/*/
Box* box;

void setup() {  
  box = new Box("", SSID, PASSWD, DEV, nullptr);
}

void loop() {
  box->cycle();
}
//*/