
#define enable D0

// ARDUINO UNO
//int values[4][4] = { {207, 171, 130, 80}, 
//                     {204, 165, 115, 64},
//                     {186, 145, 91,  37},
//                     {175, 134, 71,  9} };

// NODEMCU
int values[4][4] = { {330, 274, 210, 130}, 
                     {324, 265, 186, 102},
                     {298, 234, 151,  62},
                     {280, 216, 117,  20} };
char keys[4][4] = {  {'1', '2', '3', 'A'},
                     {'4', '5', '6', 'B'},
                     {'7', '8', '9', 'C'},
                     {'*', '0', '#', 'D'} };

char readKeypad(int);
char getKeyPress(int, long, int);
char getUniqueKey(int);

#define BOARD arduino

void setup() {
  // put your setup code here, to run once:
  pinMode(enable, OUTPUT);
  pinMode(A0, INPUT);
  Serial.begin(9600);

  digitalWrite(enable, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println(getKeyPress(3, 1000, A0));
  char ret = getKeyPress(3, 100, A0);
  //char ret = getUniqueKey(A0);
  if(ret != 'u') Serial.println(ret);
  delay(100);
}

bool within(int real, int expected, int delta){
  return real > expected - delta && real < expected + delta;
}

char getUniqueKey(int pin = A0){
  char val = getKeyPress(3, 500, pin);
  while(getKeyPress(3, 500, pin) == val);
  return(val);
}

#define SPACE 5
char getKeyPress(int stable = 3, long timeout = 1500, int pin = A0) {
  char val;
  long start = millis();
  
  val = readKeypad(pin);
  delay(SPACE);
  int i;
  for( i = 0; i <= stable && start + timeout > millis(); i++) {
    char temp = readKeypad(pin);
    if( i == stable && val != 'x') {
      return(val);   
    } else if( temp != val ) {
      val = temp;
      i = 0;
    }  
    delay(SPACE);
  }
  return('u');
}

char readKeypad(int pin = A0) {
  int tmp = analogRead(pin);
  for( int col = 0; col < 4; col++){
    for( int row = 0; row < 4; row++) {
      if( within(tmp, values[row][col], 4) ) {
        return(keys[row][col]);
      }
    }
  }
  return 'x';
}

