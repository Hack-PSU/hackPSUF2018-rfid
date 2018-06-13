#include <LiquidCrystal_I2C.h>

// Construct an LCD object and pass it the
// I2C address, width (in characters) and
// height (in characters). Depending on the
// Actual device, the IC2 address may change.
LiquidCrystal_I2C lcd(0x3F, 16, 2);

void setup() {

  // The begin call takes the width and height. This
  // Should match the number provided to the constructor.
  lcd.begin(16,2);
  lcd.init();

  // Turn on the backlight.
  lcd.backlight();

  // Move the cursor characters to the right and
  // zero characters down (line 1).
  lcd.setCursor(5, 0);

  // Print HELLO to the screen, starting at 5,0.
  lcd.print("HELLO");

  // Move the cursor to the next line and print
  // WORLD.
  lcd.setCursor(5, 1);
  lcd.print("WORLD");
}

void loop() {
  printDebug("Food", 88);
  delay(1000);
  printName("Pranav");
  delay(1000);
  printSize("M");
  delay(1000);
  printScan(true);
  delay(1000);
  printLocation("Food");
  delay(1000);
  printRegistered("True");
  delay(1000);
  printMsg("HackPSU");
  delay(1000);
}

void printDebug(String location, int wifi){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Loc: " + location + ", " + wifi + "%");
}

void printName(String name){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hi " + name);
}


void printSize(String size){
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Size: " + size);
}

void printScan(bool goodScan){
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Allow in: " + goodScan);
}

void printLocation(String location){
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Location: " + location);
}

void printRegistered(String registered){
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Registered: " + registered);
}


void printMsg(String msg){
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(msg);
}
