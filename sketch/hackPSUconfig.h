#ifndef CONFIG__H
#define CONFIG__H

#define MASTER_KEY "GET_REKT"
#define SCAN_TIMEOUT 1200
#define BAUD_RATE 9600
//#define SECURE_BOX

// RFID Pins (SPI pins)
#define RFID_RST D4         // SDD2 Configurable, see typical pin layout above
#define RFID_SS  D8         // SDD3 Configurable, see typical pin layout above

// LCD Pins (I2C pins)
#define LCD_SCL  D1
#define LCD_SDA  D2         

// Keypad pins
#define KPD_SRC  A0
#define KPD_CLK  D0
#define KPD_SIG  D3

//Uncomment this line if the SSID & PASSWDS fields are correct
//#define WIFI_CONSTS
#ifndef WIFI_CONSTS
#error Wifi constants must be updated
#endif

#define LCD
#define SERIAL

#define STATIC
// #define DYNAMIC

namespace hackPSU{
  #define  HTTPS true
  #define  PORT  443

  constexpr char* HOST   = "";
  constexpr char* SSID   = "";
  constexpr char* PASSWD = "";
  
  // SHA1 fingerprint of the certificate
  #if HTTPS
    constexpr uint8_t FP[20] = {0xAD, 0x0E, 0xA5, 0xF9, 0xAB, 0x6A, 0xEF, 0xB1, 0x25, 0x3A, 0xA4, 0x47, 0x3D, 0xA5, 0x75, 0x1A, 0xE9, 0x8C, 0xA7, 0xB5};
  #endif

  typedef struct{
    String name;
    uint32_t id;
  } Location;

}

#endif
