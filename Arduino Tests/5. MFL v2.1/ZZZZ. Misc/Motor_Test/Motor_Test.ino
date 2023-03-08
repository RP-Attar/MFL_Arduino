#include <AccelStepper.h>

#define stepPinA  33
#define dirPinA   31
#define motorInterfaceType 1

int stepRate = 100;
int stepNum = 600;
int motorSpeed = 380;

AccelStepper stepperA = AccelStepper(motorInterfaceType, stepPinA, dirPinA);

void setup() {
    stepperA.setMaxSpeed(motorSpeed);
}

void loop() {
    stepperA.setCurrentPosition(0);      // Set the current position to 0
    
    stepperA.setSpeed(stepRate);
    
    while ((stepperA.currentPosition() != stepNum)) { 
        stepperA.runSpeed();
    }
}
