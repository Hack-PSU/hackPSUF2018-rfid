#pragma once

#include<Arduino.h>

#include <esp8266/esp8266.hpp>
#include <4X4MATRIX/keypad.h>
#include <HD44780/display.h>
#include <MFRC522/rfid.h>

#include <list.hpp>
#include "menu.h"

#define MENU_STATES 10

namespace hackPSU{

  typedef enum {LOCK, MENU, DUPLICATE, ZEROIZE, WIFI, LOCATION, CHECKIN, SCAN_EVENT, GETUID, UPDATE, ITEM_CHECKOUT, ITEM_RETURN, SCAN_ITEM} State_e;
  typedef enum {UNDEFINED, EXCELLENT, GOOD, FAIR, WEAK} SignalStrength;

  class Box{

    public:
      Box(String redis_addr, const char* ssid, const char* password, Mode_e mode, const byte* band_key=nullptr);
      ~Box();

      /**
       * Call this to allow the Box to run.  Best if called in ``loop()'' or in a for(;;)/while(true) loop
       */
      void cycle();

    private:
      List<Event>* event_list;
      List<Item>* item_list;
      List<MenuItem>* menu_list;

      Event* event;
      Item* item;

      uint32_t last_scan;

      SignalStrength strength;

      Scanner*  scanner;
      Keypad*   keypad;
      Api*      http;
      Display*  display;

      /**
       * handler
       *
       * Description:
       *   Handles HTTP codes that can be handled the same way after all calls
       *
       * Codes supported:
       *  <   0
       * == 401
       * != 200
       *  > 500
       */
      bool handler(int code);

      /**
       * Description:
       *    Box is locked until a RFID tag is scanned with the secret key in the right location
       *    Blocks execution until unlocked
       *
       * State transition:
       *    MENU - on master tag read
       */
      void lock();

      /**
       * Description:
       *    Displays all control options
       *    1) Set location and scan
       *       * scan attendees into specified location
       *    2) Check-in
       *       * Special flow is required for checkin
       *    3) Wifi status
       *       * displayes connected SSID and signal strength
       *    4) Clone master tag
       *       * Duplicates the master granting access to other people
       *    5) Lock device
       *       * Locks the device again until master tag is scanned
       *
       * Controls:
       *   Numbers select menu item (for ease)
       *   # selects current item
       *
       *
       *  State transitions:
       *    LOCATION - on menu selection
       *    WIFI - on menu selection
       *    DUPLICATE - on menu selection
       *    CHECKIN - on menu selection
       *    INIT - on menu selection or invalid state
       */
      void menu();
      void menu_cleanup();

      /**
       * Description:
       *    Set the location of the box
       *
       * Controls:
       *    # selects current item
       *    A/B up and down
       *    C scrolls the LCD
       *    D returns to the menu
       *
       * State transitions:
       *    SCAN - on selection of location
       *    MENU - on 'D' press
       */
      void location();
      void location_cleanup();

      /**
       * Description:
       *    Scan members into location after location is set
       *
       * Controls:
       *    B Back button
       *    D Locks device
       *
       * State transistions:
       *    MENU - on 'B' press
       *    LOCK - on 'D' press
       */
      void scan_event();

      /**
       * Description:
       *    Handle check-in process which will include band association
       *
       * Controls:
       *    # confirm
       *    * reject
       *    C scroll LCD
       *    D lock device
       *
       *
       *
       * State transitions:
       *   MENU - on 'B' press
       *   LOCK - on 'D' press
       */
      void checkin();

      /**
       * Description:
       *    Handle item checkout which sends a band UID and a item id.
       *
       * Controls:
       *    # - select item
       *    D - lock screen
       */
      void item_checkout();

      void item_return();

      void item_cleanup();

      void scan_item(bool checkout = true);

      /**
       * Description:
       *    Display WiFi diagnostics
       *
       * State transitions:
       *   MENU - on 'B' press
       *   LOCK - on 'D' press
       */
      void wifi();

      /**
       * Description:
       *    Duplicates the master key onto a RFID tag.  Only do this for tags you wish to grant access to the system
       *
       * State transitions:
       *   MENU - on 'B' press
       *   LOCK - on 'D' press
       */
      void duplicate();

      /**
       * Description:
       *    Revoks master status of wristband
       */
      void zeroize();

      /**
       *
       */
      void getuid();

      void update();
  };
}
