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

// Create Instance of Stepper library
Stepper stepperA(stepsPerRevolution, AIN1_PIN, AIN2_PIN, AIN3_PIN, AIN4_PIN);
Stepper stepperB(stepsPerRevolution, BIN1_PIN, BIN2_PIN, BIN3_PIN, BIN4_PIN);

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
  // Step one revolution in one direction:
  Serial.println("clockwise");
  stepperA.step(stepsPerRevolution);
  stepperB.step(stepsPerRevolution);
  delay(500);

  // Step one revolution in the other direction:
  Serial.println("counterclockwise");
  stepperA.step(-stepsPerRevolution);
  stepperB.step(-stepsPerRevolution);
  delay(500);
}
