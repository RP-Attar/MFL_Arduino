// Include the Arduino Stepper Library
#include <Stepper.h>

// Stepper motors setup
#define AIN1_PIN    31
#define AIN2_PIN    33
#define AIN3_PIN    35
#define AIN4_PIN    37
#define BIN1_PIN    30
#define BIN2_PIN    32
#define BIN3_PIN    34
#define BIN4_PIN    36
const int stepsPerRevolution = 200;
const int stepsPerMove = 2;
// Create Instance of Stepper library
Stepper stepperA(stepsPerRevolution, AIN1_PIN, AIN2_PIN, AIN3_PIN, AIN4_PIN);
Stepper stepperB(stepsPerRevolution, BIN1_PIN, BIN2_PIN, BIN3_PIN, BIN4_PIN);

// Set up joystick
#define JOYX_PIN A0
int joyxValue;

void setup()
{
  // Set the speed at 60 rpm:
  stepperA.setSpeed(40);
  stepperB.setSpeed(40);
  
  // Initialize the serial port:
  Serial.begin(9600);
}

void loop() 
{
    readJoy();
    moveA();
    idleA();
    moveB();
    idleB();
}

void readJoy() {
    joyxValue = analogRead(JOYX_PIN);
}

void moveA() {
    if (joyxValue > 525) {
        stepperA.step(stepsPerMove);
    } else if (joyxValue < 510) {
        stepperA.step(-stepsPerMove);
    }
}

void moveB() {
    if (joyxValue > 525) {
        stepperB.step(stepsPerMove);
    } else if (joyxValue < 510) {
        stepperB.step(-stepsPerMove);  
    }
}

void idleA() {
  digitalWrite(30, LOW);
  digitalWrite(32, LOW);
  digitalWrite(34, LOW);
  digitalWrite(36, LOW);
}

void idleB() {
  digitalWrite(31, LOW);
  digitalWrite(33, LOW);
  digitalWrite(35, LOW);
  digitalWrite(37, LOW); 
} 
