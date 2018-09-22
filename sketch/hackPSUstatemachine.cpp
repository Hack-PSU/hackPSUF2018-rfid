#include "hackPSUstatemachine.h"

namespace hackPSU {

Box::Box(String redis_addr, const char* ssid, const char* password, Mode_e mode, const byte* band_key) {
  menu_state = 0;
  scanner = new Scanner(RFID_SS, RFID_RST);
  http =    new HTTPImpl(redis_addr);
  display = new Display(mode);
  keypad =  new Keypad(KPD_SRC, KPD_CLK, KPD_SIG, display);
  rssi_old = 0xFFFFFFFF;

  //Hit up that wifi boi
  while (!WiFi.begin(ssid, password))
    yield();
}

Box::~Box() {
  delete scanner;
  delete keypad;
  delete http;
  delete display;
  Serial.println("Deleted all Box members");
}

void Box::cycle(void) {
  //switch on state; default to init
  switch (state) {
    case INIT:
      init();
      break;
    case MENU:
      menu();
      break;
    case DUPLICATE:
      duplicate();
      break;
    case WIFI:
      wifi();
      break;
    case LOCATION:
      location();
      break;
    case CHECKIN:
      checkin();
      break;
    case SCAN:
      scan();
      break;
    default:
      state = INIT;
      break;
  }
}

void Box::menu() {

  display->print("A:UP, B:DOWN", 0);
  switch(menu_state){
    case 0:
      display->print("1:Set & Scan", 1);
      break;
    case 1:
      display->print("2:WiFi info", 1);
      break;
    case 2:
      display->print("3:Clone Master", 1);
      break;
    case 3:
      display->print("4:Checkin", 1);
      break;
    case 4:
      display->print("5:Lock", 1);
      break;
    default:
      menu_state = 0;
      state = INIT;
      return;
  }

  switch (keypad->getUniqueKey(500)) {
    case 'A':
      menu_state++;
      menu_state %=5;
      break;
    case 'B':
      menu_state--;
      menu_state %= 5;
      break;
    case '1':
      state = LOCATION;
      menu_state = 0;
      break;
    case '2':
      state = WIFI;
      menu_state = 0;
      break;
    case '3':
      state = DUPLICATE;
      menu_state = 0;
      break;
    case '4':
      state = CHECKIN;
      menu_state = 0;
      break;
    case '5':
      state = INIT;
      menu_state = 0;
      break;
  }

}

void Box::wifi(void) {

  char buff[16] = {0};
  int32_t rssi;

  if (WiFi.status() != WL_CONNECTED) {
    display->print("WiFi Disconnect", 0);
    return;
  }
  byte available_networks = WiFi.scanNetworks();
  for (int network = 0; network < available_networks; network++) {
    if (WiFi.SSID(network), String(SSID)) {
      rssi = WiFi.RSSI(network);
      break;
    }
  }

  //Only print on significant change
  if ( abs(rssi - rssi_old ) <= 5) {
    sprintf("RSSI %d dBm", buff, rssi);
    display->print(SSID, 0);
    display->print(buff, 1);
    rssi_old = rssi;
  }

  if (keypad->getUniqueKey(500) == 'B')
    state = MENU;

}

void Box::scan(void) {

  char input = keypad->readKeypad();

  switch (input) {
    //B -> escape up
    case 'B':
      state = MENU;
      return;
    //C -> escape out
    case 'C':
      state = INIT;
      return;
    //Normal behavior = scan band
    default:
      Serial.println("Please scan wristband");
      uint32_t uid = scanner->getUID();
      char lid_buffer[10] = {0};
      char uid_buffer[10] = {0};
      Serial.println(uid);
      if (uid != last_scan) {
        if (http->entryScan(itoa(lid, lid_buffer, 10), itoa(uid, uid_buffer, 10))) {
          //TODO LCD->print Allow
          Serial.println("Allow");
        } else {
          //TODO LCD->print Deny
          Serial.println("Deny");
        }
        last_scan = uid;
      }
      //No state transition
  }

}



void Box::checkin(void) {

  String pin;
  redisData* data = nullptr;
  char keyPress;

  display->print("Enter a pin", 0);
  pin = keypad->getPin(5, '*', '#', 10000);

  //Timeout
  if (pin[0] == 't'){
    delete data;
    return;
  }

  //Character press
  if (pin[0] > 64) {
    switch(pin[0]){
      case 'B':
        state = MENU;
        delete data;
        return;
      case 'D':
        state = INIT;
        delete data;
        return;
    }
  }

  //We have a pin
  data = http->getDataFromPin(pin);

  display->print("Validate name:", 0);
  display->print(data->name, 1);

  do{
    keyPress = keypad->getUniqueKey(5000);
  } while (keyPress != '#' && keyPress != '*');

  if (keyPress ==  '*'){
    delete data;
    return;
  }

  display->print("Shirt Size:", 0);
  display->print(data->shirtSize, 1);

  delay(2000);

  delete data;

}

void location(void){



}


}
