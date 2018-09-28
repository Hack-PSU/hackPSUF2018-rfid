#include <ESP8266WiFi.h>

const char* ssid = "";
const char* password = "";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("you're not connected");
    delay(1500);
  }
  Serial.print("IP is ");
  Serial.println(WiFi.localIP());
}
