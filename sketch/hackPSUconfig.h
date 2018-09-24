#ifndef CONFIG__H
#define CONFIG__H

#define MASTER_KEY "GET_REKT"

namespace hackPSU{
  constexpr char* REDIS  = "c-98-235-90-5.hsd1.pa.comcast.net:3000";
  constexpr char* SSID   = "McDermott";
  constexpr char* PASSWD = "mathematical";
  //TODO add fingerprint of redis cert

  typedef struct{
    String name;
    uint32_t id;
  } Location;

}

#endif
