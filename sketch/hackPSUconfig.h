#ifndef CONFIG__H
#define CONFIG__H

#define MASTER_KEY "GET_REKT"
#define SCAN_TIMEOUT 5000

namespace hackPSU{
  constexpr char* REDIS  = "c-98-235-90-5.hsd1.pa.comcast.net:3000";
  constexpr char* SSID   = "Sushrut's iPhone";
  constexpr char* PASSWD = "qazwsx123";
  // SHA1 fingerprint of the certificate
  constexpr char* fingerprint = "AD:0E:A5:F9:AB:6A:EF:B1:25:3A:A4:47:3D:A5:75:1A:E9:8C:A7:B5";
  constexpr uint8_t fp[20] = {0xAD, 0x0E, 0xA5, 0xF9, 0xAB, 0x6A, 0xEF, 0xB1, 0x25, 0x3A, 0xA4, 0x47, 0x3D, 0xA5, 0x75, 0x1A, 0xE9, 0x8C, 0xA7, 0xB5};

  typedef struct{
    String name;
    uint32_t id;
  } Location;

}

#endif
