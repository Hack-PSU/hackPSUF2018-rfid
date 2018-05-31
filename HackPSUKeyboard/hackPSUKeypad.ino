// Included from https://github.com/Chris--A/Keypad
#include <Keypad.h>

const byte n_rows = 4;
const byte n_cols = 4;

// Setup keypad mapping
char keys[n_rows][n_cols] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

byte colPins = []; // TODO: Fill
byte rowPins = []; // TODO: Fill

// Construct a Keypad Object
// and register the keymapping
// with pins mapping
Keypad hackPSUKeypad = Keypad(
        makeKeymap(keys),
        rowPins,
        colPins,
        n_rows,
        n_cols);

/**
 * Setup the keypad connection
 */
void setup() {
    Serial.begin(115200);
}

/**
 * Run a loop and get input every
 * tick
 */
void loop() {

}

/**
 * Get the key from the keypad
 * @return {char} The pressed key from the keypad else NULL
 */
char getKey() {
    return hackPSUKeypad.getKey();
}


