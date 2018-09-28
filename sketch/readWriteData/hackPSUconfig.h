#ifndef CONFIG__H
#define CONFIG__H

namespace hackPSU{
  constexpr char* REDIS  = "192.168.0.11:3000";//"c-98-235-90-5.hsd1.pa.comcast.net";
  constexpr char* SSID   = "";
  constexpr char* PASSWD = "";
  constexpr char* MASTER_KEY = "GET_REKT";

  typedef struct{
    String name;
    uint32_t id;
  } Location;

}

#endif
