#include <network.h>

using namespace hackPSU;

Network *net;

int getApiPin(){
  Serial.print("Type in Pin: ");
  while(!Serial.available()){yield();}
  return Serial.parseInt();
}

void setup() {
  delay(2000);
  Serial.begin(9600);
  net = new Network(REDIS);
  Serial.println("Hi");
}

void loop() {
  if ( WiFi.status() != WL_CONNECTED) {
    Serial.print("Trying to connect to ");
    Serial.println(NETWORK_SSID);
    Serial.println(NETWORK_PASSWORD);
    Serial.println(REDIS);
    // wait 10 seconds for connection:
    delay(2000);
  }else{
    Serial.println("Connected");
    //while(true){yield();}
    int pin = getApiPin();
    API::Response resp = net -> getApiKey(pin);
    Serial.println("made it here!");
    Serial.println(String(resp));
    //Serial.println(API::Response[resp]);
    String name = net -> getDataFromPin(512);
    Serial.print("Name we got from Redis: ");
    Serial.println(name);
    String wid = "RFID";
    Serial.print("THE WID IS: ");
    Serial.println(wid);
    String message = net -> assignUserWID(512, wid);
    Serial.print("This is the message after assign: ");
    Serial.println(message);
    while(true){yield();}
  }
}
