#include "hackPSUrfid.h"
#include <stdio.h>
#include <stdlib.h>

using namespace hackPSU;

Scanner* scanner;

void setup() {
  // put your setup code here, to run once:
  scanner = new Scanner(D8, D4);
  Serial.begin(115200);
  Serial.println("HI THERE");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  Serial.println("SCAN ME UP BOI");
  Serial.println(scanner->getUID());
  Serial.println("SCANNED");
  Serial.println("SCAN ME UP DADDY");
  scanner->setUID(millis());
  Serial.println(scanner->getUID());
  Serial.println("SCANNED");
  
}
