#ifndef CONFIG__H
#define CONFIG__H

namespace hackPSU{
  constexpr char* REDIS  = "c-98-235-90-5.hsd1.pa.comcast.net";
  constexpr char* SSID   = "psuwirelesssetup";
  constexpr char* PASSWD = "";
  constexpr char* MASTER_KEY = "GET_REKT";

  typedef struct{
    String name;
    uint32_t id;
  } Location;

}

#endif
