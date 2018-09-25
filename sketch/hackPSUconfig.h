#ifndef CONFIG__H
#define CONFIG__H

#define MASTER_KEY "GET_REKT"
#define SCAN_TIMEOUT 5000

namespace hackPSU{
  constexpr char* REDIS  = "c-98-235-90-5.hsd1.pa.comcast.net:3000";
  constexpr char* SSID   = "Sushrut's iPhone";
  constexpr char* PASSWD = "qazwsx123";
  // SHA1 fingerprint of the certificate
  constexpr char* fingerprint = "EF 8D DC 15 D8 FA EB 7C 32 D4 AD 25 F0 2A 5D 15 98 43 33 34";

  typedef struct{
    String name;
    uint32_t id;
  } Location;

}

#endif
