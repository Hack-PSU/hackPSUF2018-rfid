
#define enable 9
void getKey(int);

void setup() {
  // put your setup code here, to run once:
  pinMode(enable, OUTPUT);
  pinMode(A0, INPUT);
  Serial.begin(9600);

  digitalWrite(enable, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  getKey(A0);
  delay(500);
}

bool within(int real, int expected, int delta){
  return real > expected - delta && real < expected + delta;
}

void getKey(int pin = A0) {
  int tmp = analogRead(pin);
  if(      within(tmp, 141, 3) ) Serial.print("A - ");
  else if( within(tmp, 104, 3) ) Serial.print("B - ");
  else if( within(tmp,  65, 5) ) Serial.print("C - ");
  else if( within(tmp,  33, 3) ) Serial.print("D - ");
  else if( within(tmp, 225, 3) ) Serial.print("0 - "); // 3
  else if( within(tmp, 337, 3) ) Serial.print("1 - "); //x
  else if( within(tmp, 282, 3) ) Serial.print("2 - ");
  else if( within(tmp, 218, 3) ) Serial.print("3 - "); // 0
  else if( within(tmp, 326, 5) ) Serial.print("4 - ");
  else if( within(tmp, 267, 3) ) Serial.print("5 - ");
  else if( within(tmp, 114, 3) ) Serial.print("6 - ");//x
  else if( within(tmp, 298, 3) ) Serial.print("7 - ");
  else if( within(tmp, 236, 3) ) Serial.print("8 - ");
  else if( within(tmp, 154, 3) ) Serial.print("9 - ");
  else if( within(tmp, 175, 3) ) Serial.print("* - ");//x 2
  else if( within(tmp, 118, 3) ) Serial.print("# - ");//x
  else Serial.print("x - ");
  Serial.print(String(tmp));
  Serial.print('\n');  
}

