#include "hackPSUrfid.h"
#include <SPI.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

using namespace hackPSU;

Scanner* scanner;

void setup() {
  scanner = new Scanner(D8, D4);
  SPI.begin();
  Serial.begin(115200);
}


#define BUFFER_SIZE 16
void loop() {

  char buffer[BUFFER_SIZE] = {'\0'};
  itoa(millis(), buffer, 10);

  Serial.println("UID:");
  Serial.println(scanner->getUID());
  Serial.println("SET DATA: " + String(buffer));
  
  scanner->setData((byte*)buffer, BUFFER_SIZE, 4);
  
  Serial.println("DATA SET");
  
  memset(buffer, 0, BUFFER_SIZE);
  
  Serial.println("READ DATA: " + String(buffer));
  
  scanner->getData((byte*)buffer, BUFFER_SIZE*2, 4);
  
  Serial.println("READ DATA: " + String(buffer));
  Serial.println();
  delay(1000);
  
}
