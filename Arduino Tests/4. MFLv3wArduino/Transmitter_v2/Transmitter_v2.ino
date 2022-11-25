// Tx - the master

//=========== 
// Libraries to include
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <AccelStepper.h>

//===========
// Wireless Comms Setup
#define CE_PIN    39
#define CSN_PIN   41
#define IRQ_PIN  3

const byte addresses[][6] = {"00001", "00002"};   // Addresses for write/read

struct Hall_Data {                                // Max size of this struct is 32 bytes - NRF24L01 buffer limit
  int comp1 = 0;
  int comp2 = 0;
  int timeDiff = 0;
  int stepDiff = 0;
};

struct Instructions {                             // Max size of this struct is 32 bytes - NRF24L01 buffer limit
  char dir = 'S';
  bool rec = false;
  bool clippedOn = false;
};

bool newDataSend = false;                         // True when there is new data to send
bool newDataReceived = false;                     // True when there is new data received

RF24 radio(CE_PIN, CSN_PIN);
Hall_Data hallData;
Instructions surpriseMsg;

//===========
// ADC/Hall Set Up
#define ADC_SCL   21
#define ADC_SDA   20
Adafruit_ADS1115 ads;                             // Use this for the 16-bit version
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
int motorSpeed = 380;

int stepsSinceLastRead = 0;
char prevDir = 'S';
bool prevRec = false;
bool prevClippedOn = false;

// Create a new instances of the AccelStepper class:
AccelStepper stepperA = AccelStepper(motorInterfaceType, stepPinA, dirPinA);
AccelStepper stepperB = AccelStepper(motorInterfaceType, stepPinB, dirPinB);

//===========
// Timing set up
unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 1000; // send once per second

//===========

void setup() {
    // Start up serial monitor 
    Serial.begin(2000000);

    // Start up the radio obeject
    Serial.println("SimpleRx Starting");
    if (!radio.begin()) {                   // Begin the radio
        Serial.println(F("radio hardware not responding!"));
        while (1) {} // hold program in infinite loop to prevent subsequent errors
    }                          
    radio.setDataRate( RF24_2MBPS );        // Set the data rate (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS)
    radio.setRetries(1,4);                  // Delay (multiples of 250us - (n+1)*250), Count (amount of retries)
    radio.openWritingPipe(addresses[0]);    // Opens a pipe to write to
    radio.openReadingPipe(0, addresses[1]); // Opens a pipe to read from
    radio.maskIRQ(1, 1, 0);                 // Means that only "data received" will trigger interrupt on falling edge
    pinMode(IRQ_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(IRQ_PIN), newData, FALLING);
    //radio.printDetails();  

    // Start up the ADC
    ads.setDataRate(RATE_ADS1115_860SPS);   // fastest
    ads.setGain(GAIN_TWOTHIRDS);            //+/- 6.144V  1 bit = 0.1875mV (default)
    if (!ads.begin()) {
      Serial.println("Failed to initialize ADS.");
      while (1);
    }

    // Start up the stepper motors with a max RPM
    stepperA.setMaxSpeed(800);              // Set the maximum speed in steps per second
    stepperB.setMaxSpeed(800);
}

//=============

void loop() {
    currentMillis = millis();
    actData();
    moveMotors(stepsPerRevolution, stepsPerMove);
    if (currentMillis - prevMillis >= txIntervalMillis) {
        radio.stopListening();
        makeData();
        sendDataFast();
        prevMillis = millis();
        radio.startListening();
    }
}

//==============
// Makes the message that will be sent using 'sendData()'
// 'data' is a global struct
void makeData() {
    readTime = micros();
    hallData.comp1 = ads.readADC_Differential_0_1();
    hallData.comp2 = random(32000);//ads.readADC_Differential_2_3();;
    hallData.timeDiff = readTime - prevReadTime;
    hallData.stepDiff = stepsSinceLastRead;
    newDataSend = true;
    stepsSinceLastRead = 0;
    prevReadTime = readTime;
}

//==============
// Sends the data made using 'makeData()', and displays it in the serial monitor 
// 'data' is a global struct
void sendDataSlow() {
    if (newDataSend == true) {
      radio.write(&hallData, sizeof(Hall_Data));
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
}

//==============
// Sends the data made using 'makeData()' 
// 'data' is a global struct
void sendDataFast() {
    if (newDataSend == true) {
      radio.write(&hallData, sizeof(Hall_Data));
    }
}

//==============
// Sets a flag when the IRQ pin is triggered (hence the interrupt)
// 'data' is a global struct
void newData() {
    bool tx_ds, tx_df, rx_dr;
    radio.whatHappened(tx_ds, tx_df, rx_dr); // resets the IRQ pin to HIGH (to make .available reliable)
    if (radio.available()) {
      newDataReceived = true;
    }
}

//==============
// Shows the obtained data, if the interupt was triggered since the last display
// 'surpriseMsg' is a global variable
void showData() {
    if (newDataReceived == true) {
        if (radio.available()) {
            radio.read(&surpriseMsg, sizeof(Instructions)); // Read the data and store it into the 'surpriseMsg' structure;
        }
        Serial.println("Data received: ");
        Serial.print("direction: ");
        Serial.println(surpriseMsg.dir);
        Serial.print("record: ");
        Serial.println(surpriseMsg.rec);
        Serial.print("collector ring closed: ");
        Serial.println(surpriseMsg.clippedOn);
        Serial.println();
        newDataReceived = false;
    }
}

//==============
// Acts on the obtained data, if the interupt was triggered since the last display
// 'surpriseMsg' is a global variable
void actData() {
    if (newDataReceived == true) {
        if (radio.available()) {
            radio.read(&surpriseMsg, sizeof(Instructions)); // Read the data and store it into the 'surpriseMsg' structure;
        }
        if (surpriseMsg.rec != prevRec) {
            if (surpriseMsg.rec == true) {
              Serial.println("New File"); 
            } else {
              Serial.println("End File");
            }
            prevRec = surpriseMsg.rec;
        }
        if (surpriseMsg.clippedOn != prevClippedOn){
            // rotate servo in this section
            if (surpriseMsg.clippedOn == true) {
              Serial.println("Servo Open"); 
            } else {
              Serial.println("Servo Close");
            }
            prevClippedOn = surpriseMsg.clippedOn;
        }
        newDataReceived = false;
    }
}

//==============
// Moves motors by motorSpeed steps/sec by stepsPerMove steps each time it is called
void moveMotors(int stepRate, int stepNum) {
    stepperA.setCurrentPosition(0);      // Set the current position to 0
    stepperB.setCurrentPosition(0);      // Set the current position to 0

    if (surpriseMsg.dir == 'L') {
        stepperA.setSpeed(stepRate);
        stepperB.setSpeed(stepRate);
        while ((stepperB.currentPosition() != stepNum)){// && (stepperA.currentPosition() != stepNum)) {
          stepperA.runSpeed();
          stepperB.runSpeed();
          stepsSinceLastRead = stepsSinceLastRead + 1;
        }  
    } else if (surpriseMsg.dir == 'R') {
        stepperA.setSpeed(-stepRate);
        stepperB.setSpeed(-stepRate);
        while ((stepperB.currentPosition() != -stepNum)){// && (stepperA.currentPosition() != -stepNum)) {
          stepperA.runSpeed();
          stepperB.runSpeed();
          stepsSinceLastRead = stepsSinceLastRead + 1;
        }  
    } else {
        stepperA.setSpeed(0);
        stepperB.setSpeed(0);
    }
    
}
