#pragma once

#include<cstring>
#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<ESP8266WiFi.h>

#include "httpwrapper.h"
#include "hackPSUkeypad.h"
#include "hackPSUdisplay.h"
#include "hackPSUrfid.h"
#include "hackPSUconfig.h"


#define GOLDEN_KEY 0xC0DEBABE
#define SECURE_BOX

// RFID Pins
#define RFID_RST 2          // SDD2 Configurable, see typical pin layout above
#define RFID_SS  15         // SDD3 Configurable, see typical pin layout above

// LCD Pins (I2C pins)
#define LCD_SCL  5
#define LCD_SDA  4

// Keypad pins
#define KPD_SRC  A0
#define KPD_CLK  D0
#define KPD_SIG  D3

namespace hackPSU{

  typedef enum State {LOCK, MENU, DUPLICATE, WIFI, LOCATION, CHECKIN, SCAN} State_e;
  typedef enum {UNDEFINED, EXCELLENT, GOOD, FAIR, WEAK} SignalStrength;

  class Box{
    private:
      Location* location_list;
      int num_locations;
      int location_state;
      
      State_e state;
      uint32_t lid; // Location id
      uint32_t last_scan;

      SignalStrength strength;

      uint8_t menu_state;


      Scanner*  scanner;
      Keypad*   keypad;
      HTTPImpl* http;
      Display*  display;
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
       *  State transitions:
       *    LOCATION - on menu selection
       *    WIFI - on menu selection
       *    DUPLICATE - on menu selection
       *    CHECKIN - on menu selection
       *    INIT - on menu selection or invalid state
       */
      void menu();

      /**
       * Description:
       *    Set the location of the box
       *    
       * State transitions:
       *    SCAN - on selection of location
       *    MENU - on 'D' press
       */
      void location();

      /**
       * Description:
       *   Scan members into location
       *   
       * State transistions:
       *   MENU - on 'B' press
       *   LOCK - on 'D' press
       */
      void scan();

      /**
       * Description:
       *    Handle check-in process which will include band association
       *    
       * State transitions:
       *   MENU - on 'B' press
       *   LOCK - on 'D' press
       */
      void checkin();

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
      

    public:
      Box(String redis_addr, const char* ssid, const char* password, Mode_e mode, const byte* band_key=nullptr);
      ~Box();

      void cycle();
  };
}

