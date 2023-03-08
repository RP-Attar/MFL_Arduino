// For the Mega

//=========== 
// Libraries to include
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <AccelStepper.h>

//=========== 
// Set up struct to store all the data from the Hall Effect Sensors
// Max size of this struct is 32 bytes 
struct Hall_Data {                              
  int comp1 = 0;
  int comp2 = 0;
  int timeDiff = 0;
  int stepDiff = 0;
};

Hall_Data hallData;

//=========== 
// Set up struct to store intructions from the Pi
// Max size of this struct is 32 bytes                          
char dir = 'S';
bool rec = false;
bool clippedOn = false;

//===========
// ADC/Hall Set Up
#define ADC_SCL   21
#define ADC_SDA   20
Adafruit_ADS1115 ads; // use this for the 16-bit version
unsigned long readTime;
unsigned long prevReadTime = 0;

//===========
// Stepper motors setup
// Motor interface type must be set to 1 when using a driver:
#define stepPinA  33
#define dirPinA   31
#define stepPinB  32
#define dirPinB   30
#define motorInterfaceType 1

const int stepsPerRevolution = 200;
const int stepsPerMove = 1;
int motorSpeed = 150;

int stepsSinceLastRead = 0;

// Create a new instances of the AccelStepper class:
AccelStepper stepperA = AccelStepper(motorInterfaceType, stepPinA, dirPinA);
AccelStepper stepperB = AccelStepper(motorInterfaceType, stepPinB, dirPinB);

//===========
// Timing set up
unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 1000; // send once per second

void setup() {
    // Start up serial comms
    Serial.begin(115200);
    while (!Serial) {}

    // Start up the ADC
    ads.setDataRate(RATE_ADS1115_860SPS);     // fastest
    ads.setGain(GAIN_TWOTHIRDS);                    // +/- 6.144V  1 bit = 0.1875mV (default)
    if (!ads.begin()) {
      Serial.println("Failed to initialize ADS.");
      while (1);
    }

    // Start up the stepper motors with a max RPM
    stepperA.setMaxSpeed(800);                         // Set the maximum speed in steps per second
    stepperB.setMaxSpeed(800);
}

void loop() {
    // Gathering data and sending
    currentMillis = millis();
    if (currentMillis - prevMillis >= txIntervalMillis) {
        makeData();
        sendDataSlow();
        prevMillis = currentMillis;
    }

    // Receiving data and acting
    if (Serial.available() > 0) {
        String message = Serial.readStringUntil('\n');
        //Serial.println(message);
        actSurprised(message);
    }
    moveMotors(motorSpeed, stepsPerMove);
}


//==============
// Makes the message that will be printed to the serial monitor using sendDataSlow() 
// 'data' is a global struct
void makeData() {
    readTime = micros();
    hallData.comp1 = ads.readADC_Differential_0_1();
    hallData.comp2 = random(32000);//ads.readADC_Differential_2_3();;
    hallData.timeDiff = readTime - prevReadTime;
    hallData.stepDiff = stepsSinceLastRead;
    stepsSinceLastRead = 0;
    prevReadTime = readTime;
}

//==============
// Sends the data made using 'makeData()' by displaying it in the serial monitor 
// 'data' is a global struct
void sendDataSlow() {
    Serial.println("Data sent: ");
    Serial.print("comp1: ");
    Serial.println(hallData.comp1);
    Serial.print("comp2: ");
    Serial.println(hallData.comp2);
    Serial.print("timeDiff: ");
    Serial.println(hallData.timeDiff);
    Serial.print("stepDiff: ");
    Serial.println(hallData.stepDiff);
    Serial.println();
}

//==============
// Acts on the obtained data, if the interupt was triggered since the last display
void actSurprised(String message) {
    if (message == "a") {
        dir = 'L';
    } else if (message == "d") {
        dir = 'R';
    } else if (message == "s") {
        dir = 'S';
    }
}

//==============
// Moves motors by stepsPerMove steps each time it is called
void moveMotors(int stepRate, int stepNum) {
    stepperA.setCurrentPosition(0);      // Set the current position to 0
    stepperB.setCurrentPosition(0);      // Set the current position to 0
    
    if (dir == 'L') {
        stepperA.setSpeed(stepRate);
        stepperB.setSpeed(stepRate);
        while ((stepperB.currentPosition() != stepNum)) { //&& (stepperA.currentPosition() != stepNum)) {
          stepperA.runSpeed();
          stepperB.runSpeed();
          stepsSinceLastRead = stepsSinceLastRead + 1;
        }  
    } else if (dir == 'R') {
        stepperA.setSpeed(-stepRate);
        stepperB.setSpeed(-stepRate);
        while ((stepperB.currentPosition() != -stepNum)) { //&& (stepperA.currentPosition() != -stepNum)) {
          stepperA.runSpeed();
          stepperB.runSpeed();
          stepsSinceLastRead = stepsSinceLastRead + 1;
        }  
    } else if (dir == 'S') {
        stepperA.setSpeed(0);
        stepperB.setSpeed(0);
    } 
}
