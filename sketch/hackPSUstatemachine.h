#pragma once

#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>

#include "httpwrapper.h"
#include "hackPSUkeypad.h"
//#include "hackPSUdisplay.h"
#include "hackPSUrfid.h"

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
      typedef struct {
        String name;
        uint32_t id;
      } Location;

      Location* location_list;
      uint16_t num_locations;
      
      State_e state;
      uint32_t lid; // Location id
      uint32_t last_scan;


      Scanner*    scanner;
      Keypad*  keypad;
      HTTPImpl* http;
      
      //hackPSUdisplay* display;

      void init();
      void menu();
      void duplicate();
      void wifi();
      void location();
      void checkin();
      void scan();      
      

    public:
      Box(String redis_addr, const char* ssid, const char* password, const byte* band_key=nullptr);
      ~Box();

      void cycle();
  };
}

