#include "hackPSUstatemachine.h"

namespace hackPSU{

#define MENU_STATES 5

Box::Box(String redis_addr, const char* ssid, const char* password, Mode_e mode, const byte* band_key) {
  // Create class objects
  scanner = new Scanner(RFID_SS, RFID_RST);
  http =    new HTTPImpl(redis_addr);
  display = new Display(mode);
  keypad =  new Keypad(KPD_SRC, KPD_CLK, KPD_SIG, display);

  // Set default values
  menu_state = 0;
  location_state = 0;
  strength = UNDEFINED;
  state = LOCK;

  display->print("WiFi connecting", 0);
  while (!WiFi.begin(ssid, password))
    yield();

  display->print("Fetching API key", 1);
  while(!http->getAPIKey()){
    yield();
  }
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
    case LOCK:
      lock();
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
      state = LOCK;
      break;
  }
}

/*
  
*/
void Box::lock(){

    #ifdef SECURE_BOX
      byte buffer[READ_BUFFER] = {0};

      display->print("Scan to unlock", 0);
      display->clear(1);
      
      scanner->getData(buffer, READ_BUFFER, KEY_BLOCK);
  
      if(String(MASTER_KEY) == String((char*)buffer)){
        state = MENU;
      } else {
        display->print("Invalid band", 1);
      }
    #else
      display->print("INSECURE BOOT!", 0);
      delay(1000);
      state = MENU;
    #endif
}

void Box::menu() {
  display->print("A:UP, B:DOWN", 0);
  switch(menu_state){
    case 0:
      display->print("1:Set & Scan", 1);
      break;
    case 1:
      display->print("2:Check-In", 1);
      break;
    case 2:
      display->print("3:WiFi info", 1);
      break;
    case 3:
      display->print("4:Clone Master", 1);
      break;
    case 4:
      display->print("5:Lock", 1);
      break;
    default:
      menu_state = 0;
      state = LOCK;
      return;
  }

  switch (keypad->getUniqueKey(500)) {
    case 'A':
      menu_state = (MENU_STATES + menu_state - 1) % MENU_STATES;
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
      state = CHECKIN;
      menu_state = 0;
      break;
    case '3':
      state = WIFI;
      menu_state = 0;
      break;
    case '4':
      state = DUPLICATE;
      menu_state = 0;
      break;
    case '5':
      state = LOCK;
      menu_state = 0;
      break;
    case '#':
      switch(menu_state){
        case 0: 
          state = LOCATION;
          break;
        case 1:
          state = CHECKIN;
          break;
        case 2:
          state = WIFI;
          break;
        case 3:
          state = DUPLICATE;
          break;
        case 4: 
          state = LOCK;
          break;
        default:
          state = LOCK;
      }
      menu_state = 0;
  }

}

void Box::location(){
  
  // Do not select location based on a number
  switch (keypad->getUniqueKey(500)) {
    case 'A':
      location_state++;
      location_state %= num_locations;
      break;
    case 'B':
      location_state = (num_locations + location_state - 1) % num_locations;
      break;
    case 'C':
       // TODO display->scroll();
       break;
    case 'D':
      location_state = 0;
      delete location_list;
      location_list = nullptr;
      state = MENU;
      break;
    case '#':
      lid = location_list[location_state].id;
      delete location_list;
      location_list = nullptr;
      state = SCAN;
      break;
    default:
      if (location_list == nullptr) {
        display->print("Updating list", 1);
        location_list = http->getLocations(num_locations);
        location_state = 0;
      }
      
      if(num_locations > 0){
        display->print("A:UP, B:DOWN", 0);
        display->print(location_list[location_state].name, 1);
      } else {
        display->print("No locations found", 1);
        delete location_list;
        location_list = nullptr;
        return;
      }
  }





}

void Box::scan() {

  char input = keypad->readKeypad();

  switch (input) {
    //B -> escape up
    case 'B':
      location_state = 0;
      state = MENU;
      return;
    //C -> escape out
    case 'D':
      location_state = 0;
      state = LOCK;
      return;
    //Normal behavior = scan band
    default:
      display->print("Scan wristband", 0);
      display->clear(1);
      uint32_t uid = scanner->getUID();
      char lid_buffer[10] = {0};
      char uid_buffer[10] = {0};
      Serial.println(uid);
      if (uid != last_scan) {
        if (http->entryScan(itoa(lid, lid_buffer, 10), itoa(uid, uid_buffer, 10))) {
          //TODO LCD->print Allow
          display->print("Allow", 1);
          delay(1000);
        } else {
          //TODO LCD->print Deny
          display->print("Deny", 1);
          delay(1000);
        }
        last_scan = uid;
      }
      //No state transition
  }

}

void Box::checkin() {

  String pin;
  redisData* data = nullptr;
  char keyPress;

  display->print("*:CLEAR #:SUBMIT", 0);
  display->print("Pin: ", 1);
  pin = keypad->getPin(5, '*', '#', 10000);

//Character press
  switch(pin[0]){
    case 'B':
      state = MENU;
      return;
    case 'D':
      state = LOCK;
      return;
    case 't': // timeout
      return;
  }

  //We have a pin
  data = http->getDataFromPin(pin);
  if (data == nullptr){
    display->print("Invalid pin", 0);
    delay(1000);
    return;
  }

  display->print("Validate name:", 0);
  display->print(data->name, 1);

  do{
    keyPress = keypad->getUniqueKey(5000);
    if(keyPress == 'C'){
      // TODO: display->scroll();
    }
    //TODO faster with scroll
  } while (keyPress != '#' && keyPress != '*');

  if (keyPress ==  '*'){ // if not expected person
    delete data;
    return;
  }

  uint32_t uid;
  display->print("Scan wristband", 0);
  do {
    display->clear(1);
    
    uid = scanner->getUID();
    if (!http->assignRfidToUser(String(uid), pin)){
      display->print("Scrap wristband!", 1);
      uid = 0;
      delay(2000);
    }
    
  } while(!uid);


  display->print("Shirt Size: ", 0);
  display->print(data->shirtSize);

  display->print("Photo concent?",1);

  // Require keypress to continue
  while(keypad->getUniqueKey(1000) != 't');

  delete data;
}

void Box::wifi() {
  
  display->print("B: RETURN", 0);
  switch(keypad->getUniqueKey(500)){
    case 'B':
      state = MENU;
      strength = UNDEFINED;
      break;
    case 'D':
      state = LOCK;
      strength = UNDEFINED;
      break;
    default:
      int32_t rssi;
      if (WiFi.status() != WL_CONNECTED) {
        display->print("WiFi Disconnect", 1);
        return;
      }
      
      rssi = WiFi.RSSI();
    
      if(rssi > -50) {
        display->print("Excellent signal", 1);
        strength = EXCELLENT;
      }  else if(rssi > -60) {
        display->print("Good signal", 1);
        strength = GOOD;
      } else if( rssi > -70) {
        display->print("Fair signal", 1);
        strength = FAIR;
      } else {
        display->print("Weak signal", 1);
        strength = WEAK;
      }
  }



}

void Box::duplicate() {

  display->print("B: BACK", 0);
  display->print("Scan Target", 1);

  switch(keypad->getUniqueKey(100)){
    case 'B': 
      state = MENU;
      break;
    case 'D':
      state = LOCK;
      break;
    default:
      // TODO: write master wristband flow (if present)
      break;
  }

  byte write_buffer[WRITE_BUFFER] = MASTER_KEY;

  scanner->setData(write_buffer, WRITE_BUFFER, KEY_BLOCK);

  display->print("Target written", 0);
  display->print("Rescan to validate", 1);

  byte read_buffer[READ_BUFFER] = {0};

  scanner->getData(read_buffer, READ_BUFFER, KEY_BLOCK);

  Serial.println(String((char*)read_buffer));

  display->clear();
  if (String((char*)read_buffer) == String(MASTER_KEY)) {
    display->print("Write success!", 1);
  } else {
    display->print("Write failure!", 1);
  }

  delay(1000);

  display->print("B: RETURN", 0);
  display->print("Wait to loop", 1);

  char key = keypad->getUniqueKey(1000);

  if (key == 'B'){
    state = MENU;
  }
  
}


}
