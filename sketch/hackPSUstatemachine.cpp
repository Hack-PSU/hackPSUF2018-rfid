#include "hackPSUstatemachine.h"

namespace hackPSU{

#define MENU_STATES 5

Box::Box(String redis_addr, const char* ssid, const char* password, Mode_e mode, const byte* band_key) {
  menu_state = 0;
  scanner = new Scanner(RFID_SS, RFID_RST);
  http =    new HTTPImpl(redis_addr);
  display = new Display(mode);
  keypad =  new Keypad(KPD_SRC, KPD_CLK, KPD_SIG, display);
  rssi_old = 0xFFFFFFFF;

  // Wait for wifi to connect
  while (!WiFi.begin(ssid, password)){
    display->print("Connecting...", 0);
    yield();
  }
  
  while(!http->getAPIKey()){
    display->print("Connecting...", 0);
    delay(1000);
  }
}

Box::~Box() {
  delete scanner;
  delete keypad;
  delete http;
  delete display;
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
      //duplicate();
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
      menu_state = MENU_STATES + menu_state - 1;
      menu_state %= MENU_STATES;
      break;
    case 'B':
      menu_state++;
      menu_state %= MENU_STATES;
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
    case '#':
      switch(menu_state){
        case 0: 
          state = LOCATION;
          break;
        case 1:
          state = WIFI;
          break;
        case 2:
          state = DUPLICATE;
          break;
        case 3:
          state = CHECKIN;
          break;
        case 4:
          state = INIT;
          break;
        default:
          state = INIT;
      }
      menu_state = 0;
  }

}

void Box::wifi(void) {
  
  int32_t rssi;

  if (WiFi.status() != WL_CONNECTED) {
    display->print("WiFi Disconnect", 0);
    return;
  }

  rssi = WiFi.RSSI();
  //Only print on significant change
  if ( abs(rssi - rssi_old ) >= 5) {
    display->print(SSID, 0);
    display->print("RSSI " + String(rssi) + " dBm", 1);
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

void Box::duplicate(void) {
  //GET REKT
  Serial.println("DUPE");
  state = MENU;
}

void Box::checkin(void) {

  String pin;
  redisData* data = nullptr;
  char keyPress;

  display->print("Enter a pin", 0);
  pin = keypad->getPin(5, '*', '#', 10000);
  Serial.println(pin);
  //Timeout
  if (pin[0] == 't'){
    return;
  }

  //Character press
  if (pin[0] > 64) {
    switch(pin[0]){
      case 'B':
        state = MENU;
        return;
      case 'D':
        state = INIT;
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

void Box::location(void){

  if (location_list == nullptr) {
    location_list = http->getLocations(&num_locations);
    location_state = 0;
  }

  display->print("A:UP, B:DOWN", 0);
  display->print(location_list[location_state].name, 1);

  char key = keypad->getUniqueKey(5000);

  switch (key) {
    case 'A':
      location_state++;
      location_state %= num_locations;
      break;
    case 'B':
      location_state++;
      location_state %= num_locations;
      break;
    case '#':
      lid = location_list[location_state].id;
      delete location_list;
      state = SCAN;
    default:
      return;
  }

  if (key <= '9' && key >= '1'){
    lid = location_list[key-'1'].id;
    delete location_list;
    state = SCAN;
  }

}
void Box::init(void){
  state = MENU;
}


}
