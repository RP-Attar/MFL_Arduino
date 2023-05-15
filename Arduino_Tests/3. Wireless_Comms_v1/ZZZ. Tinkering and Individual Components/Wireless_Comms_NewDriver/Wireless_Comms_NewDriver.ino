// Include the AccelStepper library:
#include <AccelStepper.h>

// Define stepper motor connections and motor interface type. 
// Motor interface type must be set to 1 when using a driver:
#define stepPin 6
#define dirPin 7
#define motorInterfaceType 1
const int stepsPerRevolution = 200;

// Create a new instance of the AccelStepper class:
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

void setup() {
  stepper.setMaxSpeed(1000);          // Set the maximum speed in steps per second
  stepper.setCurrentPosition(0);      // Set the current position to 0
}

void loop() { 

  // Run the motor forward at 200 steps/second until the motor reaches 400 steps (2 revolutions):
  while(stepper.currentPosition() != 400)
  {
    stepper.setSpeed(200);
    stepper.runSpeed();
  }

  delay(1000);

  // Run the motor backwards at 600 steps/second until the motor reaches -200 steps (1 revolution):
  while(stepper.currentPosition() != -200) 
  {
    stepper.setSpeed(-600);
    stepper.runSpeed();
  }

  delay(1000);

  // Run the motor forward at 400 steps/second until the motor reaches 600 steps (3 revolutions):
  while(stepper.currentPosition() != 600)
  {
    stepper.setSpeed(400);
    stepper.runSpeed();
  }

  delay(3000);
}
