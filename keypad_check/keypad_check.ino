
int COLS[] = {0,4,5,16}; // D3, D2, D1, D0
             
char readKeypad(int);
char getKeyPress(int, long, int);
char getUniqueKey(int);

#define BOARD arduino

void setup() {
  // put your setup code here, to run once:
  pinMode(A0, INPUT);
  for(int col : COLS)
    pinMode(col, OUTPUT);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  char key = readKeypad(A0);
  if(key != 'x') Serial.println(key);
  delay(100);
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
  char key = 'x';
  for( int i = 0; i < 4 && key == 'x'; i++){
    int col = COLS[i];
    digitalWrite(col, HIGH);
    int tmp = analogRead(pin);
    if( tmp > 800 ) key = keys[0][i];
    else if( tmp > 450 ) key = keys[1][i];
    else if( tmp > 320 ) key = keys[2][i];
    else if( tmp > 50 )  key = keys[3][i];
    digitalWrite(col, LOW);
  }
  return key;
}

