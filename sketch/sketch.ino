
#include "hackPSUrfid.h"
#include "hackPSUstatemachine.h"
#include <ESP8266WiFi.h>
#include "hackPSUhttp.h"
#include "httpwrapper.h"
#include "hackPSUkeypad.h"
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
using namespace hackPSU;

void dump_byte_array(byte*, byte);

#define clearSR() shiftOut(KPD_SIG, KPD_CLK, MSBFIRST, B00000000);

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


const byte I2C_ADDRESSES[] = {0x27, 0x3f};
LiquidCrystal_I2C *lcd;
MFRC522 mfrc522(RFID_SS, RFID_RST);  // Create MFRC522 instance

void putChar(char character) {
  lcd->print(character);
  Serial.println("Character printed: " + character);
}
void clear(){
  Serial.println("CLEAR SENT");
  lcd->clear();
  lcd->setCursor(1,0);
}
Keypad keypad(KPD_SRC, KPD_CLK, KPD_SIG, &clear, &putChar);

//Scanner scanner(RFID_SS, RFID_RST);
//StateMachine fsm();


void setup() {  
  // USE_SERIAL.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
   String ssid = "";//inster ssid here
   String password = "";//insert password here
   //WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  Headers headers [] = { { "Content-Type", "application/json" } };
  Response* r = HTTP::GET("http://httpbin.org/get");
  Response* x = HTTP::POST("http://httpbin.org/post","{\"dat\": \"boi\"}",1, headers);

  //HTTPImpl* http = new HTTPImpl("192.168.137.29:3000");
  //redisData* data = http->getDataFromPin("1234");
  
  Serial.println(r->payload);
  Serial.println(r->responseCode);
  Serial.println(r->errorStrings);
  Serial.println(x->payload);
  Serial.println(x->responseCode);
  Serial.println(x->errorStrings);
  //Serial.println(data->shirtSize);
  //Serial.println(data->diet);

  delete r;
  delete x;
  
  // Differences
              
  // INIT Serial communication
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Begin of program");
  
  // INIT LCD/I2C communication
  Wire.begin();
  for(byte i : I2C_ADDRESSES){
    Wire.beginTransmission(i);
    if(Wire.endTransmission() == 0){
      lcd = new LiquidCrystal_I2C(i, 16, 2);
      Serial.print("I2C LCD found at 0x");
      Serial.println(i, HEX);
      break;
    }
  }
  
  lcd->begin();
  lcd->clear();
  lcd->backlight();
  lcd->setCursor(0,0);
  lcd->print("Hello!");
  
  lcd->setCursor(0,1);
  lcd->print("Scan wristband");
  
  // INITI SPI/scanner communication
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
    
}

void loop() {
  // Look for new cards
  
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    delay(50);
    return;
  // Select one of the cards  
  }

  if ( ! mfrc522.PICC_ReadCardSerial()) {
    delay(50);
    return;
  }
  
  lcd->clear();
  lcd->setCursor(0,0);
  lcd->print("UID:");
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  
  lcd->setCursor(0,1);
  
  String pin = keypad.getPin(10, '*','#', 15000);
  Serial.println("PIN received: " + pin);
  delay(500);
  lcd->clear();
  lcd->setCursor(0,0);
  lcd->print("Hello!");
  
  lcd->setCursor(0,1);
  lcd->print("Scan wristband");
  
}


// Helper routine to dump a byte array as hex values to Serial
void dump_byte_array(byte *buffer, byte bufferSize) {
  lcd->print("0x");
  for (byte i = 0; i < bufferSize; i++) {
    if(buffer[i] < 0x10){
      lcd->print("0");
    }
    lcd->print(buffer[i], HEX);
  }

}
