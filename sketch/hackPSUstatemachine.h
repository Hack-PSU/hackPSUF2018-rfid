#pragma once

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

  typedef enum State {INIT, MENU, DUPLICATE, WIFI, LOCATION, CHECKIN, SCAN} State_e;

  class Box{
    private:
      Location* location_list;
      int num_locations;
      int location_state;
      
      State_e state;
      uint32_t lid; // Location id
      uint32_t last_scan;

      uint32_t rssi_old;

      uint8_t menu_state;


      Scanner*  scanner;
      Keypad*   keypad;
      HTTPImpl* http;
      Display*  display;
      
      //hackPSUdisplay* display;

      void init();
      void menu();
      void duplicate();
      void wifi();
      void location();
      void checkin();
      void scan();      
      

    public:
      Box(String redis_addr, const char* ssid, const char* password, Mode_e mode, const byte* band_key=nullptr);
      ~Box();

      void cycle();
  };
}

