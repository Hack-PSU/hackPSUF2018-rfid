#include "hackPSUdisplay.h"
using namespace hackPSU;

Display *display;

void setup() {
  // put your setup code here, to run once:
  display = new Display(DEV);
  display->print('A', CHECK);
  display->print('B', LOCK);
  display->print('C', CLEAR);
  display->print('D', BACK);
  delay(3000);
  display->print("Hello my name is Sushrut Shringarputale Tech Director for 2018", 0);
}

void loop() {
  // put your main code here, to run repeatedly:
    delay(5000);
    display->scroll();
}
