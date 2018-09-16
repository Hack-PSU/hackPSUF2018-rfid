#include "hackPSUrfid.h"
#include "hackPSUstatemachine.h"
#include <ESP8266WiFi.h>
#include "hackPSUhttp.h"
#include "httpwrapper.h"
#define USE_SERIAL Serial
using namespace hackPSU;

void setup() {
    
  USE_SERIAL.begin(115200);
  // USE_SERIAL.setDebugOutput(true);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }
   ssid = //inster ssid here
   password = //insert password here
   WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  Headers headers [] = { { "Content-Type", "application/json" } };
  Response* r = HTTP::GET("http://httpbin.org/get");
  Response* x = HTTP::POST("http://httpbin.org/post","{\"dat\": \"boi\"}",1, headers);

  //HTTPImpl* http = new HTTPImpl("192.168.137.29:3000");
  //redisData* data = http->getDataFromPin("1234");
  
  Serial.println(r->payload);
  Serial.println(r->responseCode);
  Serial.println(r->errorStrings);
  Serial.println(x->payload);
  Serial.println(x->responseCode);
  Serial.println(x->errorStrings);
  //Serial.println(data->shirtSize);
  //Serial.println(data->diet);

  delete r;
  delete x;

}

void loop() {
}


