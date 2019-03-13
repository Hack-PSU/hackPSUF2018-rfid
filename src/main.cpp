#include <network.h>
#include <config.h>

using namespace hackPSU;

Network *net;

int getApiPin(){
  Serial.print("Type in Pin: ");
  while(!Serial.available()){yield();}
  return Serial.parseInt();
}

void setup() {
  delay(2000);
  Serial.begin(9600);
  net = new Network(REDIS);

  Serial.print("Trying to connect to ");
  Serial.println(NETWORK_SSID);
  Serial.println(NETWORK_PASSWORD);
  Serial.println(REDIS);
  while( WiFi.status() != WL_CONNECTED){
    delay(2000);
  }

  Serial.println("Connected");

}

void loop() {

  int apipin = getApiPin();

  // Get API key
  HTTPCode resp = net->getApiKey(apipin);
  if(resp) {
    Serial.println("Fetch API key success");
  } else {
    Serial.println("Fetch API key failed");
    return;
  }

  // Get User data
  int userpin = getApiPin();

  User pinuser = net->getDataFromPin(userpin);
  Serial.print("Name we got from Redis: ");
  Serial.println(pinuser.name);

  // Setting wid of user to RFID
  String wid = "RFID";
  HTTPCode message = net->assignUserWID(userpin, wid);
  if(message){
    Serial.println("Success");
  }

  // Lookup user using new WID
  User widuser = net->userInfoFromWID(wid);
  Serial.print("User name: ");
  Serial.println(widuser.name);

  // Get locations
  Locations locations = net->getEvents();
  Serial.println("These are the locations: ");
  Serial.println(locations.length);
  for(int i=0; i < locations.length; i++){
    Serial.println(locations.data[i].name);
  }


  User validation = net->sendScan(wid, 5);
  Serial.println("This is the user we scanned in: ");
  Serial.println(validation.name);
  Serial.println(validation.shirtSize);
  Serial.println(validation.diet);
  Serial.println(validation.allow);


  while(true){yield();}
}
