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
  if(http->getApiKey() == hackPSU::API::SUCCESS){
    Serial.println("Successfully acquired API key");
  } else {
    Serial.println("Failed acquiring API key");
  }

  http->enableOTA();
  Serial.println("Begin OTA");
}
void loop(){
  http->handleOTA();
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