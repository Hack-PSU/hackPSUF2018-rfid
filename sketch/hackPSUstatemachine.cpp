#include "hackPSUstatemachine.h"

namespace hackPSU{


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
  last_scan = 0;

  display->print("WiFi connecting", 0);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) 
    yield();

  display->print("Connected...", 0);
  display->print("Fetching API key", 1);

  while(!http->getAPIKey()){
    yield();
  }
  display->clear();
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
    case LOCK:
      lock();
      return;
    case MENU:
      menu();
      return;
    case DUPLICATE:
      duplicate();
      return;
    case WIFI:
      wifi();
      return;
    case LOCATION:
      location();
      return;
    case CHECKIN:
      checkin();
      return;
    case ZEROIZE:
      zeroize();
      return;
    case SCAN:
      scan();
      return;
    default:
      state = LOCK;
      return;
  }
}

void Box::lock(){

    #ifdef SECURE_BOX
      display->print("Scan to unlock", 0);

      byte buffer[READ_BUFFER] = {0};
      if(GOOD_RF != scanner->getData(buffer, READ_BUFFER, KEY_BLOCK, SCAN_TIMEOUT))
        return;
  
      if(String(MASTER_KEY) == String((char*)buffer)){
        state = MENU;
        display->clear();
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
      display->print("Set & Scan", 1);
      break;
    case 1:
      display->print("Check-In", 1);
      break;
    case 2:
      display->print("WiFi info", 1);
      break;
    case 3:
      display->print("Clone Master", 1);
      break;
    case 4:
      display->print("Zeroize", 1);
      break;
    case 5:
      display->print("Lock", 1);
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
    case 'D':
      menu_cleanup();
      state = LOCK;
      break;
    case '1':
      state = LOCATION;
      menu_cleanup();
      break;
    case '2':
      state = CHECKIN;
      menu_cleanup();
      break;
    case '3':
      state = WIFI;
      menu_cleanup();
      break;
    case '4':
      state = DUPLICATE;
      menu_cleanup();
      break;
    case '5':
      state = ZEROIZE;
      menu_cleanup();    
    case '6':
      state = LOCK;
      menu_cleanup();
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
          state = ZEROIZE;
          break;
        case 5:
          state = LOCK;
          break;
        default:
          state = LOCK;
      }
      menu_cleanup();
  }
}

void Box::menu_cleanup(){
  menu_state = 0;
  display->clear();

}

void Box::location(){
  display->print("A:UP, B:DOWN", 0);
  
  // Do not select location based on a number
  switch (keypad->getUniqueKey(500)) {
    case 'A':
      location_state++;
      location_state %= num_locations;
      return;
    case 'B':
      location_state = (num_locations + location_state - 1) % num_locations;
      return;
    case 'C':
       // TODO display->scroll();
       return;
    case 'D':
      location_cleanup();
      state = MENU;
      return;
    case '#':
      lid = location_list[location_state].id;
      location_name = location_list[location_state].name;
      location_cleanup();
      state = SCAN;
      return;
    default:
      if (location_list == nullptr) {
        display->print("Updating list", 1);
        location_list = http->getLocations(num_locations);
        location_state = 0;
      }
      
      if(num_locations > 0){
        display->print("A:UP, B:DOWN", 0);
        display->print(location_list[location_state].name.c_str(), 1);
      } else {
        display->print("No locations found", 1);
        delay(2000);
        location_cleanup();
      }
  }
}

void Box::location_cleanup(){
  if(location_list != nullptr){
    delete location_list;
    location_list = nullptr;
  }
  location_state = 0;
  display->clear();
}

void Box::scan() {
  uint32_t uid = 0;
  char lid_buffer[10] = {0};
  char uid_buffer[10] = {0};
  display->print("Scan wristband", 0);

  char input = keypad->getUniqueKey(1500);
  switch (input) {
    case 'C':
      last_scan = 0; // reset last_scan
    case 'D':
      last_scan = 0;
      state = LOCK;
      return;
    //Normal behavior = scan band
    default:
      uid = scanner->getUID(SCAN_TIMEOUT);
      if (uid && uid != last_scan) {
        if (http->entryScan(itoa(lid, lid_buffer, 10), itoa(uid, uid_buffer, 10))) {
          display->print("Allow", 1);
        } else {
          display->print("Deny", 1);
        }
        delay(750);
        last_scan = uid;
        display->clear(1);
      }
  }
}

/**
 * First half is getting registrant's pin 
 * Second half is associating registrant to wristband  
 */
void Box::checkin() {
  display->print("*:CLEAR #:SUBMIT", 0);

  String pin;
  RedisData* data = nullptr;
  char keypress;
  uint32_t uid;

  display->print("Enter pin: ", 1);
  pin = keypad->getPin(6, '*', '#', 10000);

//Character press
  switch(pin[0]){
    case 'A':
    case 'B': 
    case 'C': 
      display->print("Invalid command", 1);
      delay(1000);
      break;
    case '\0':
      display->print("Invalid pin", 1);
      delay(1000);
      return;
    case 'D':
      state = LOCK;
      display->clear();
      if(data != nullptr) delete data;
      return;
    case 't': // timeout
      return;
  }

  data = http->getDataFromPin(pin);
  if (data == nullptr){
    display->print("Invalid pin", 1);
    delay(2000);
    return;
  }

  display->print("Validate name:", 0);
  display->print(data->name, 1);
  
  // Enter next half of checkin: associating registrant with wristband
  keypress = keypad->getUniqueKey(5000);
  bool validated = false;
  do{
    switch(keypress){
      case '*': // Wrong person
        if( !validated){
          if (data != nullptr) delete data;
          return;
        }
        break;
      case 'D':
        if (data != nullptr) delete data;
        state = LOCK;
        display->clear();
        return;
      case 'C':
        // TODO: display->scroll();
        break;
      case '#': // Name validated
        validated = true;
        break;
      case 'A':
      case 'B':
        if (data != nullptr) delete data;
        display->clear();
        return;
    }
    if(validated){
      display->print("Scan wristband", 0);
      display->clear(1);
      uid = scanner->getUID(SCAN_TIMEOUT);
      if( uid && !http->assignRfidToUser(String(uid), pin)) {
        display->print("Scrap wristband!", 1);
        uid = 0;
        delay(2000);
      } 
    }
    keypress = keypad->getUniqueKey(1200);
  } while (!uid);

  display->print("Shirt Size: ", 0);
  display->print(data->shirtSize);  // TODO: Get shirt size correctly

  display->print("Photo consent?",1);

  // Require keypress to continue
  while(keypad->getUniqueKey(5000) == 't');

  delete data;
  data = nullptr;
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
  RfidState lastState = GOOD_RF;
  display->print("D:LOCK", 0);
  display->print("Scan Target", 1);

  byte write_buffer[WRITE_BUFFER] = MASTER_KEY;
  byte read_buffer[READ_BUFFER] = {0};

  switch(keypad->getUniqueKey(2000)){
    case 'D':
      state = LOCK;
      display->clear();
      break;
    default:
      switch(scanner->setData(write_buffer, WRITE_BUFFER, KEY_BLOCK, SCAN_TIMEOUT)){
        case GOOD_RF:
          display->print("Target written", 0);
          display->print("Rescan target", 1);
          while((lastState = scanner->getData(read_buffer, READ_BUFFER, KEY_BLOCK, SCAN_TIMEOUT)) == TIMEOUT){
            yield();
          }
          switch(lastState){
              case READ_FAIL:
                display->print("Read Failure!", 1);
                break;
              case CRYPTO_FAIL:
                display->print("Crypo Failure!", 1);
               break;
              case GOOD_RF:
                if (String((char*)read_buffer) == String(MASTER_KEY)) {
                  display->print("Write success!", 1);
                } else {
                  display->print("Write failure!", 1);
                }
                delay(1000);
                break;
            }
          break;
        case WRITE_FAIL:
          display->print("Write Failure!", 1);
          break;
        case CRYPTO_FAIL:
          display->print("Crypto Failure!", 1);
      }
  }
}

void Box::zeroize() {
  RfidState lastState = GOOD_RF;
  display->print("D:LOCK", 0);
  display->print("Scan Target", 1);

  byte write_buffer[WRITE_BUFFER] = {0};
  byte read_buffer[READ_BUFFER] = {0};

  switch(keypad->getUniqueKey(2000)){
    case 'D':
      state = LOCK;
      display->clear();
      break;
    default:
      switch(scanner->setData(write_buffer, WRITE_BUFFER, KEY_BLOCK, SCAN_TIMEOUT)){
        case GOOD_RF:
          display->print("Target zeroized", 0);
          display->print("Rescan target", 1);
          while((lastState = scanner->getData(read_buffer, READ_BUFFER, KEY_BLOCK, SCAN_TIMEOUT)) == TIMEOUT){
            yield();
          }
          switch(lastState){
              case READ_FAIL:
                display->print("Read Failure!", 1);
                break;
              case CRYPTO_FAIL:
                display->print("Crypo Failure!", 1);
               break;
              case GOOD_RF:
                if (String((char*)read_buffer) == String((char*)read_buffer)) {
                  display->print("Write success!", 1);
                } else {
                  display->print("Write failure!", 1);
                }
                delay(1000);
                break;
            }
          break;
        case WRITE_FAIL:
          display->print("Write Failure!", 1);
          break;
        case CRYPTO_FAIL:
          display->print("Crypto Failure!", 1);
      }
  }
}
}
