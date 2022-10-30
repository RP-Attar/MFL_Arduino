// Include the AccelStepper library:
#include <AccelStepper.h>

// Stepper motors setup
// Motor interface type must be set to 1 when using a driver:
#define stepPin 6
#define dirPin 7
#define motorInterfaceType 1
const int stepsPerRevolution = 200;
const int stepsPerMove = 2;
int motorSpeed = 400;
// Create a new instance of the AccelStepper class:
AccelStepper stepperA = AccelStepper(motorInterfaceType, stepPin, dirPin);

// Set up joystick
#define JOYX_PIN A0
int joyxValue;
int startTime;

void setup()
{
    stepperA.setMaxSpeed(1000);          // Set the maximum speed in steps per second
    
    // Initialize the serial port:
    Serial.begin(9600);
}

void loop() 
{
    //startTime = micros();
    readJoy();
    motorA();
    //Serial.println(micros()-startTime);
}

void readJoy() {
    joyxValue = analogRead(JOYX_PIN);
}

void motorA() {
    if (joyxValue > 525) {
        moveMotor(-motorSpeed, -stepsPerMove);
    } else if (joyxValue < 510) {
        moveMotor(motorSpeed, stepsPerMove);
    }
}

void moveMotor(int stepRate, int stepNum) {
    stepperA.setCurrentPosition(0);      // Set the current position to 0
    stepperA.setSpeed(stepRate);
    while(stepperA.currentPosition() != stepNum)
        {
          stepperA.runSpeed();
        }  
}
