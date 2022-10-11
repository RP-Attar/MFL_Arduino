/*
 * Code description here
 */

// Setting all pin values
const int enA = 2;
const int in1 = 3;
const int in2 = 4; 
const int btnL = 5;
const int btnR = 6;
const int hall = A0;
int hallVal = 0;
int btnLState = 0;
int btnLPrev = 0;
int btnRState = 0;
int btnRPrev = 0;

// Defining all functions 

void setup() {
  // put your setup code here, to run once:
  // begin the serial monitor
  Serial.begin(9600);

  // setting pinModes
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(btnL, INPUT);
  pinMode(btnR, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  // get button states
  btnLPrev = btnLState;
  btnRPrev = btnRState;
  btnLState = digitalRead(btnL);
  btnRState = digitalRead(btnR);

  // plot states of buttons;
  Serial.print(btnLPrev);
  Serial.print(",");
  Serial.print(btnRPrev);
  Serial.print(",");
  Serial.print(btnLState);
  Serial.print(",");
  Serial.println(btnRState);
  
  // determine what to do with motor
  if (btnLState==HIGH && btnLPrev==HIGH) {
    motorL(100);
  } else if (btnRState==HIGH && btnRPrev==HIGH) {
    motorR(100);
  } else {
    motorStop();
  }

  delay(20);
}

void motorL(int speed) { // speed should be from 0% to 100%
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  speed = map(speed, 0, 100, 0, 255);
  analogWrite(enA, speed); 
}

void motorR(int speed) { // speed should be from 0% to 100%
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  speed = map(speed, 0, 100, 0, 255);
  analogWrite(enA, speed); 
}

void motorStop(){
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(enA, 0); 
}
