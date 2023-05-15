/*
 * Code description here
 */

// Setting all pin values
const int enA = 2;
const int in1 = 3;
const int in2 = 4; 
const int btnL = 5;
const int btnR = 6;
const int btnRec = 7;
const int hall = A0;

// Setting all variables
int hallVal = 0;
int btnLState = 0;
int btnLPrev = 0;
int btnRState = 0;
int btnRPrev = 0;
int btnRecState = 0;
int btnRecPrev = 0;
int btnRecOnce = 0;
int fileNum = 1;
unsigned long currTime;
unsigned long startTime; 

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
  // check if we need to move motor
  motorBtnCheck();

  // check if we need to record
  btnRecPrev = btnRecState;
  btnRecState = digitalRead(btnRec);

  // start recording (send "new file" trigger)
  startTime = 0;
  if (btnRecPrev==1 && btnRecState==1 && btnRecOnce==0) {
    Serial.print("new file: r2_largeCut_x80_");
    Serial.println(String(fileNum));
    btnRecOnce = 1;
    fileNum = fileNum + 1;
  } else if (btnRecState==0 && btnRecPrev==0 && btnRecOnce==1) {
    btnRecOnce = 0;
    Serial.println("end file");
  }

  // run every loop
  Serial.print(millis()-currTime);
  currTime = millis();
  Serial.print(",");
  Serial.println(analogRead(hall));
  
  delay(5);
}

void motorBtnCheck() {
  // get button states
  btnLPrev = btnLState;
  btnRPrev = btnRState;
  btnLState = digitalRead(btnL);
  btnRState = digitalRead(btnR);
  
  // determine what to do with motor
  if (btnLState==HIGH && btnLPrev==HIGH) {
    motorL(100);
  } else if (btnRState==HIGH && btnRPrev==HIGH) {
    motorR(100);
  } else {
    motorStop();
  }
}

void motorR(int speed) { // speed should be from 0% to 100%
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  speed = map(speed, 0, 100, 0, 255);
  analogWrite(enA, speed); 
}

void motorL(int speed) { // speed should be from 0% to 100%
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
