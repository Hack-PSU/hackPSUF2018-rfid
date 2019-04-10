#include "fullbox.h"

#ifndef RFID_SS
#error Macro, RFID_SS, not set
#define RFID_SS 0
#endif
#ifndef RFID_RST
#error Macro, RFID_RST, not set
#define RFID_RST 0
#endif
#ifndef KPD_SRC
#error Macro, KPD_SRC, not set
#define KPD_SRC 0
#endif
#ifndef KPD_CLK
#error Macro, KPD_CLK, not set
#define KPD_CLK 0
#endif
#ifndef KPD_SIG
#error Macro, KPD_SIG, not set
#define KPD_SIG 0
#endif




namespace hackPSU{

Box::Box(String redis_addr, const char* ssid, const char* password, Mode_e mode, const byte* band_key) {
  // Create class objects
  scanner = new Scanner((char*)"scanner", RFID_SS, RFID_RST);
  if(Box::display != nullptr)  Box::display = new Display(mode);
  http    = (Api*) new ESP8266_Device((char*)"http");
  http->setAddress(REDIS);

  keypad  = new Keypad(KPD_SRC, KPD_CLK, KPD_SIG, Box::display);

  event_list = new List<Event>();
  item_list = new List<Item>();
  menu_list = new List<MenuItem>();

  MenuItem* menuItem = new MenuItem();
  //add each menu item to menu_item list
  menuItem->heading = "Lock";
  menuItem->loop = [](Box* box) {box->lock();};
  menu_list->addItem(menuItem); //memcpy in list

  menuItem->heading = "Set & Scan";
  menuItem->loop = [](Box* box) {box->location();};
  menu_list->addItem(menuItem);

  menuItem->heading = "Check-In";
  menuItem->loop = [](Box* box) {box->checkin();};
  menu_list->addItem(menuItem);

  menuItem->heading = "Item checkout";
  menuItem->loop = [](Box* box) {box->item_checkout();};
  menu_list->addItem(menuItem);

  menuItem->heading = "Item return";
  menuItem->loop = [](Box* box) {box->item_return();};
  menu_list->addItem(menuItem);

  menuItem->heading = "Show Name";
  menuItem->loop = [](Box* box) {box->getuid();};
  menu_list->addItem(menuItem);

  menuItem->heading = "WiFi info";
  menuItem->loop = [](Box* box) {box->wifi();};
  menu_list->addItem(menuItem);

  menuItem->heading = "Clone Master";
  menuItem->loop = [](Box* box) {box->duplicate();};
  menu_list->addItem(menuItem);

  menuItem->heading = "Zeroize";
  menuItem->loop = [](Box* box) {box->zeroize();};
  menu_list->addItem(menuItem);

  menuItem->heading = "Update Firmware";
  menuItem->loop = [](Box* box) {box->update();};
  menu_list->addItem(menuItem);

  // Set default values
  strength = UNDEFINED;
  last_scan = 0;

  Box::display->print("WiFi connecting", 0);
  while(WiFi.status() != WL_CONNECTED)
    yield();

  Box::display->print("Connected...", 0);
  Box::display->print("Fetching API key", 1);


  if(!http->checkApiKey()){
    Box::display->clear();
    Box::display->print("Enter API pin: ", 0);
    while(true) {
      Box::display->clear(1);
      String pin = keypad->getPin(10, '*', '#', 10000);
      if(pin != "timeout" && http->getApiKey(pin)){
        break;
      }
      yield();
    }
  } else {
    Serial.println("Successfully got API key");
  }
  Box::display->clear();
  menu_list->reset();
}

Box::~Box() {
  delete scanner;
  delete keypad;
  delete http;
  delete Box::display;
}

void Box::cycle(void) {
  //switch on state; default to init
  menu_list->getCurrent()->loop(this);
}

void Box::lock(){
  if(event_list->size() > 0) {
    event_list->clear();
    event = nullptr;
  }
  if(item_list->size() > 0) {
    item_list->clear();
    item = nullptr;
  }

    #if !defined(DEBUG) || defined(SECURE_BOX)
      Box::display->print("Scan to unlock", 1);

      byte buffer[READ_BUFFER] = {0};
      if(GOOD_RF != scanner->getData(buffer, READ_BUFFER, KEY_BLOCK, SCAN_TIMEOUT))
        return;

      if(String(MASTER_KEY) == String((char*)buffer)){
        Box::display->clear();
        menu_list->next();
      }
    #else
      Box::display->print("INSECURE BOOT!", 0);
      Box::display->print("Happy Debugging!", 1);
      delay(1000);
      menu_list->next();
    #endif

}

void Box::menu() {
  Box::display->print('A', UP_C, 'B', DOWN_C, '#', CHECK_C, 'D', LOCK_C);
  Box::display->print(menu_list->getCurrent()->heading, 1);

  switch (keypad->getUniqueKey(500)) {
    case 'A':
      menu_list->previous();
      break;
    case 'B':
      menu_list->next();
      break;
    case 'D':
      menu_cleanup();
      menu_list->reset();
      break;
    case '#':
      MenuItem *curr = menu_list->getCurrent();
      curr->loop(this);
      break;
      menu_cleanup();
    }
}

void Box::menu_cleanup(){
  Box::display->clear();
  menu_list->reset();
}

void Box::location(){
  Box::display->print('#', CHECK_C, 'B', DOWN_C, 'C', SCROLL_C, 'D', BACK_C);
  if(event != nullptr) {
    scan_event();
    return;
  }

  // Do not select location based on a number
  switch (keypad->getUniqueKey(500)) {
    case 'A':
      event_list->next();
      return;
    case 'B':
      event_list->previous();
      return;
    case 'C':
       Box::display->scroll();
       return;
    case 'D':
      location_cleanup();
      menu_list->reset();
      menu_list->next();
      return;
    case '#':
      event = event_list->getCurrent();
      return;
    default:
      if (event_list->size()  == 0) {
        Box::display->print("Updating list", 1);

        http->getEvents(event_list);
      }

      if(event_list->size() > 0){
        Box::display->print(event->name, 1);
      } else {
        Box::display->print("No locations found", 1);
        delay(2000);
        location_cleanup();
        menu_list->next();
      }
  }
}

void Box::location_cleanup(){
  Box::display->clear();
  event_list->clear();
  event = nullptr;
  menu_list->reset(); // lock
}

void Box::scan_event() {
  Box::display->print('*',NONE_C, '#', NONE_C, '\0', NONE_C, 'D', LOCK_C);

  uint32_t uid = 0;
  Box::display->print("Scan wristband", 1);

  char input = keypad->getUniqueKey(750);
  switch (input) {
    case 'C':
      last_scan = 0; // reset last_scan
      break;
    case 'D':
      last_scan = 0;
      Box::display->clear();
      location_cleanup();
      return;
    //Normal behavior = scan band
    default:
      uid = scanner->getUID(SCAN_TIMEOUT);
      if (uid && uid != last_scan) {
        Box::display->toggleDisplay();
        User* user = new User();
        int code = http->sendScan(String(uid), event, user, millis());
        switch(code) {
          case 200: Box::display->print("Success", 1);         break;
          case 404: Box::display->print("Unknown user", 1);  break;
          case 401: Box::display->print("Restarting...", 1); break;
          default:
            if(code < 0 ){
              Box::display->print("Network error", 1);
            } else {
              Box::display->print("Fail: " + http->decode(code), 1);
            }
        }
        //#error Handle entryScan
        delay(750);
        last_scan = uid;
      }
  }
}

/**
 * First half is getting registrant's pin
 * Second half is associating registrant to wristband
 */
void Box::checkin() {
  Box::display->print('*',CLEAR_C, '#', CHECK_C, 'C', SCROLL_C, 'D', LOCK_C);

  String pin;
  char keypress;
  uint32_t uid;

  Box::display->print("Enter pin: ", 1);
  pin = keypad->getPin(6, '*', '#', 10000);

  //Character press
  switch(pin[0]){
    case 'A':
    case 'B':
    case 'C':
      Box::display->print("Invalid command", 1);
      delay(1000);
      return;
    case '\0':
      Box::display->print("Invalid pin", 1);
      delay(1000);
      return;
    case 'D':
      Box::display->clear();
      menu_list->reset();
      return;
    case 't': // timeout
      return;
  }

  User* user = new User;

  int code = http->getUserInfo(PIN, pin, user);
  if(!handler(code)) {
    switch(code){
      case 404:
        Box::display->print("Invalid pin", 1);
        delay(2000);
        return;
    }
  }

  Box::display->print("Validate name:", 0);
  Box::display->print(user->name, 1);

  // Enter next half of checkin: associating registrant with wristband
  keypress = keypad->getUniqueKey(5000);
  bool validated = false;
  do{
    switch(keypress){
      case '*': // Wrong person
        if( !validated){
          return;
        }
        break;
      case 'D':
        Box::display->clear();
        menu_list->reset();
        return;
      case 'C':
        Box::display->scroll();
        break;
      case '#': // Name validated
        validated = true;
        break;
    }
    if(validated){
      Box::display->print("Scan wristband", 0);
      Box::display->clear(1);
      uid = scanner->getUID(SCAN_TIMEOUT);
      if(uid){
        int code = http->registerUser(String(uid), pin);

        // TODO: handle code

        if(code != 200){
          Box::display->print("Try again - " + String(code), 0);
          delay(2500);
          return;
        }
      }
    }
    keypress = keypad->getUniqueKey(1200);
  } while (!uid);

  Box::display->print("Shirt Size: ", 0);
  Box::display->print(user->shirtSize);

  Box::display->print("Photo consent?",1);

  // Require keypress to continue
  while(keypad->getUniqueKey(5000) == 't');

  Box::display->clear();
}

void Box::wifi() {

  Box::display->print('B', BACK_C, '\0', NONE_C, '\0', NONE_C, 'D', LOCK_C);
  switch(keypad->getUniqueKey(500)){
    case 'B':
      menu_list->reset();
      menu_list->next();
      strength = UNDEFINED;
      Box::display->clear();
      break;
    case 'D':
      strength = UNDEFINED;
      Box::display->clear();
      menu_list->reset();
      break;
    default:
      int32_t rssi;
      if (WiFi.status() != WL_CONNECTED) {
        Box::display->print("WiFi Disconnect", 1);
        return;
      }

      rssi = WiFi.RSSI();

      if(rssi > -50) {
        Box::display->print("Excellent signal", 1);
        strength = EXCELLENT;
      }  else if(rssi > -60) {
        Box::display->print("Good signal", 1);
        strength = GOOD;
      } else if( rssi > -70) {
        Box::display->print("Fair signal", 1);
        strength = FAIR;
      } else {
        Box::display->print("Weak signal", 1);
        strength = WEAK;
      }
  }
}

void Box::duplicate() {
  RfidState lastState = GOOD_RF;
  Box::display->print('\0', NONE_C, '\0', NONE_C, '\0', NONE_C, 'D', LOCK_C);
  Box::display->print("Scan Target", 1);

  byte write_buffer[WRITE_BUFFER] = MASTER_KEY;
  byte read_buffer[READ_BUFFER] = {0};

  switch(keypad->getUniqueKey(2000)){
    case 'D':
      Box::display->clear();
      menu_list->reset();
      break;
    default:
      switch(scanner->setData(write_buffer, WRITE_BUFFER, KEY_BLOCK, SCAN_TIMEOUT)){
        case GOOD_RF:
          Box::display->print("Target written", 0);
          Box::display->print("Rescan target", 1);
          while((lastState = scanner->getData(read_buffer, READ_BUFFER, KEY_BLOCK, SCAN_TIMEOUT)) == TIMEOUT){
            yield();
          }
          switch(lastState){
              case READ_FAIL:
                Box::display->print("Read Failure!", 1);
                break;
              case CRYPTO_FAIL:
                Box::display->print("Crypo Failure!", 1);
               break;
              case GOOD_RF:
                if (String((char*)read_buffer) == String(MASTER_KEY)) {
                  Box::display->print("Write success!", 1);
                } else {
                  Box::display->print("Write failure!", 1);
                }
                delay(1000);
                break;
            }
          break;
        case WRITE_FAIL:
          Box::display->print("Write Failure!", 1);
          break;
        case CRYPTO_FAIL:
          Box::display->print("Crypto Failure!", 1);
      }
  }
}

void Box::zeroize() {
  RfidState lastState = GOOD_RF;
  Box::display->print('\0', NONE_C, '\0', NONE_C, '\0', NONE_C, 'D', LOCK_C);
  Box::display->print("Scan Target", 1);

  byte write_buffer[WRITE_BUFFER] = {0};
  byte read_buffer[READ_BUFFER] = {0};

  switch(keypad->getUniqueKey(2000)){
    case 'D':
      Box::display->clear();
      menu_list->reset();
      break;
    default:
      switch(scanner->setData(write_buffer, WRITE_BUFFER, KEY_BLOCK, SCAN_TIMEOUT)){
        case GOOD_RF:
          Box::display->print("Target zeroized", 0);
          Box::display->print("Rescan target", 1);
          while((lastState = scanner->getData(read_buffer, READ_BUFFER, KEY_BLOCK, SCAN_TIMEOUT)) == TIMEOUT){
            yield();
          }
          switch(lastState){
              case READ_FAIL:
                Box::display->print("Read Failure!", 1);
                break;
              case CRYPTO_FAIL:
                Box::display->print("Crypo Failure!", 1);
               break;
              case GOOD_RF:
                if (String((char*)read_buffer) == String((char*)read_buffer)) {
                  Box::display->print("Write success!", 1);
                } else {
                  Box::display->print("Write failure!", 1);
                }
                delay(1000);
                break;
            }
          break;
        case WRITE_FAIL:
          Box::display->print("Write Failure!", 1);
          break;
        case CRYPTO_FAIL:
          Box::display->print("Crypto Failure!", 1);
      }
  }
}

void Box::getuid(){
  Box::display->print('#', CHECK_C, '\0', NONE_C, '\0', NONE_C, 'D', LOCK_C);
  Box::display->print("Scan for UID", 1);

  switch(keypad->getUniqueKey(2000)){
    case 'D':
      Box::display->clear();
      menu_list->reset();
      return;
    default:
     uint32_t uid = scanner->getUID(SCAN_TIMEOUT);
     Serial.println("UID: " + String(uid));
      if(uid){
        User* user = new User();
        int code = http->getUserInfo(WID, String(uid), user);
        Box::display->print(user->name, 1);
        while(keypad->getUniqueKey(5000) == 't');
      }
  }
}

void Box::update(){
  Box::display->print("Update Enabled.", 0);
  Box::display->print("IP:"+http->localIP(), 1);
  http->enableUpdate();
  
  switch(keypad->getUniqueKey(750)){
  case 'D':
    Box::display->clear();
    menu_list->reset();
    return;
  default:
    http->handleUpdate();
    break;
  }
}

void Box::item_checkout(){
  Box::display->print('#', CHECK_C, 'B', DOWN_C, 'C', SCROLL_C, 'D', BACK_C);
  if(item != nullptr) {
    scan_item(true);
    return;
  }

  // Do not select item based on a number
  switch (keypad->getUniqueKey(500)) {
    case 'A':
      item_list->next();
      return;
    case 'B':
      item_list->previous();
      return;
    case 'C':
      Box::display->scroll();
      return;
    case 'D':
      item_cleanup();
      menu_list->next();
      return;
    case '#':
      item = item_list->getCurrent();
      break;
    default:
      if (item_list->size()  == 0) {
        Box::display->print("Updating list", 1);

        int res = http->getItems(item_list);
      }

      if(item_list->size() > 0){
        Box::display->print(item_list->getCurrent()->name, 1);
      } else {
        Box::display->print("No items found", 1);
        delay(2000);
        item_cleanup();
        menu_list->next();
      }
  }
}

void Box::item_return(){
  Box::display->print('#', CHECK_C, 'B', DOWN_C, 'C', SCROLL_C, 'D', BACK_C);
  if(item != nullptr) {
    scan_item(false);
    return;
  }

  switch (keypad->getUniqueKey(500)) {
    case 'A':
      item = item_list->next();
      return;
    case 'B':
      item = item_list->previous();
      return;
    case 'C':
      Box::display->scroll();
      return;
    case 'D':
      item_cleanup();
      menu_list->next();
      return;
    case '#':
      item = item_list->getCurrent();
      break;
    default:
      if (item_list->size()  == 0) {
        Box::display->print("Updating list", 1);

        int res = http->getItems(item_list);

        // TODO handle response
      }

      if(item_list->size() > 0){
        Box::display->print(item_list->getCurrent()->name, 1);
      } else {
        Box::display->print("No items found", 1);
        delay(2000);
        item_cleanup();
        menu_list->next();
      }
  }
}

void Box::scan_item(bool checkout) {

  Box::display->print('*',NONE_C, '#', NONE_C, '\0', NONE_C, 'D', LOCK_C);

  uint32_t uid = 0;
  Box::display->print("Scan wristband", 1);

  char input = keypad->getUniqueKey(1500);
  switch (input) {
    case 'C':
      last_scan = 0; // reset last_scan
      break;
    case 'D':
      last_scan = 0;
      Box::display->clear();
      menu_list->reset();
      return;
    //Normal behavior = scan band
    default:
      uid = scanner->getUID(SCAN_TIMEOUT);
      if (uid && uid != last_scan) {
        //User data = http->(String(uid), lid);
        int code = checkout ? http->itemCheckout(String(uid), item) : http->itemReturn(String(uid), item);
        // TODO: handle different responses
        if (code) {
          Box::display->print("Allow", 1);
          Box::display->toggleDisplay();
        } else {
          Box::display->print("Deny - " + http->decode(code) , 1);
          delay(2500);
          return;
        }
        switch( int(code) ) {
          case 200: Box::display->print("Allow", 1);         break;
          case 404: Box::display->print("Unknown user", 1);  break;
          case 401: Box::display->print("Restarting...", 1); break;
          default:
            if(int(code) < 0 ){
              Box::display->print("Network error", 1);
            } else {
              Box::display->print("Deny - " + http->decode(code) , 1);
            }
        }
        //#error Handle entryScan
        delay(750);
        last_scan = uid;
      }
  }
}

void Box::item_cleanup(){
  item_list->clear();
  menu_list->reset();
}

  bool Box::handler(int code){
    String pin;
    if(code < 0) {
      // Hanlded codes
        Box::display->print("Reconnecting to ", 0);
        Box::display->print("the Network...", 1);
        http->connect();
        delay(5000);
        // reconnect to wifi;
        return true;
    }
    if( code == 401 ) {
        Box::display->print("Undating API key", 0);
        Box::display->print("Pin: ", 1);
        pin = keypad->getPin(10, '*', '#', 10000);
        return http->getApiKey(pin) == 200;
    }
    return false;
  }
}
