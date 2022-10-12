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
  stepperA.setSpeed(80);
  stepperB.setSpeed(80);
  
  // Initialize the serial port:
  Serial.begin(9600);
}

void loop() 
{
    readJoy();
    moveMotors();
}

void readJoy() {
    joyxValue = analogRead(JOYX_PIN);
}

void moveMotors() {
    if (joyxValue > 525) {
        stepperA.step(stepsPerMove);
        stepperB.step(stepsPerMove);
    } else if (joyxValue < 510) {
        stepperA.step(-stepsPerMove);
        stepperB.step(-stepsPerMove);      
    }
}
