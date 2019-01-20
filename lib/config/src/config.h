#ifndef CONFIG__H
#define CONFIG__H

#define MASTER_KEY "GET_REKT"
#define API_VERSION "v1.0"

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
//#error Wifi constants must be updated
#endif

#define LCD_EN
#define SERIAL_EN

#define STATIC
//#define DYNAMIC

#if defined(STATIC) && defined(DYNAMIC) || !defined(STATIC) && !defined(DYNAMIC)
  #error Cannot have both STATIC and DYNAMIC buffers
#endif

#if defined(STATIC)
  #define MAKE_BUFFER(obj_size, arr_size) StaticJsonBuffer<JSON_OBJECT_SIZE(obj_size)+JSON_ARRAY_SIZE(arr_size)>
#elif defined(DYNAMIC) 
  #define MAKE_BUFFER(obj_size, arr_size) DynamicJsonBuffer
#endif 

constexpr char* SSID   = (char*)"";
constexpr char* PASSWD = (char*)"";

namespace hackPSU{
  //#define  HTTPS

  #define HOST "";

  
  // SHA1 fingerprint of the certificate
  #ifdef HTTPS
    constexpr uint8_t FP[20] = {0xAD, 0x0E, 0xA5, 0xF9, 0xAB, 0x6A, 0xEF, 0xB1, 0x25, 0x3A, 0xA4, 0x47, 0x3D, 0xA5, 0x75, 0x1A, 0xE9, 0x8C, 0xA7, 0xB5};
  #endif

  typedef struct{
    String name;
    uint32_t id;
  } Location;

}

#endif
