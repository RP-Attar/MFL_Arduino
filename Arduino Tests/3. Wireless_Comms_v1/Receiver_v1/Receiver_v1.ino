// The slave (or the receiver)
// Should be loaded onto the Arduino MEGA

//===========

// Libraries to include
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Stepper.h>

//===========

// Wireless Set Up
#define CE_PIN  39    // nRF24
#define CSN_PIN 41    // nRF24
// Set the address we are sending info to
const byte thisSlaveAddress[5] = {'R','x','A','A','A'};
// Create a Radio object and variables for receiving 
RF24 radio(CE_PIN, CSN_PIN);
char dataReceived[32]; // This must match dataToSend in the TX
bool newData = false;

//===========

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
const int stepsPerMove = 1;
// Create Instance of Stepper library
Stepper stepperA(stepsPerRevolution, AIN1_PIN, AIN2_PIN, AIN3_PIN, AIN4_PIN);
Stepper stepperB(stepsPerRevolution, BIN1_PIN, BIN2_PIN, BIN3_PIN, BIN4_PIN);

//===========

// Recording Set Up
#define REC_PIN 2     // Button for recording
int fileNum = 1;
bool recording = LOW;

//===========

// Joystick Set Up
#define JOYX_PIN A0
int joyxValue;

//===========

void setup() {
    // Set up Serial comms
    Serial.begin(2000000);                          // Make sure this is the same accross all programs
    Serial.println("SimpleTx Starting");

    // Start up the recording button interrupt
    pinMode(REC_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(REC_PIN), recData, CHANGE);

    // Start up the radio obeject 
    radio.begin();                                  // Begin the radio
    radio.setDataRate( RF24_2MBPS );                // Set the data rate (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS)
    radio.openReadingPipe(1, thisSlaveAddress);     // Opens a pipe to read from 
    radio.startListening();                         // Begin listening for messages

    // Start up the stepper motors at X RPM
    stepperA.setSpeed(80);
    stepperB.setSpeed(80);
}

//=============

// Runs forever
void loop() {
    getData();
    showData();
    readJoy();
    moveA();
    //idleA();
    moveB();
    //idleB();
}

//==============

// Checks if there is data being sent, stores it, and sets newData flag to true
// Inputs:  NULL
// Outputs: NULL
void getData() {
    if (radio.available()) {
        radio.read(&dataReceived, sizeof(dataReceived));
        newData = true;
    }
}

//==============

// Checks if there is new data, shows it, and sets newData flag to true
// Inputs:  NULL
// Outputs: NULL
void showData() {
    if (newData == true) {
        Serial.println(dataReceived);
        newData = false;
    }
}

//==============

// Triggered by the rising or falling edge of the record button 
// Inputs:  NULL
// Outputs: NULL
void recData() {
    if (digitalRead(REC_PIN)==HIGH && recording==LOW) {           // Runs when REC_PIN is HIGH (btn pressed) and recording flag is LOW (not recording)
        // Checks for debounce
        delay(2);
        if (digitalRead(REC_PIN)==HIGH) {
            // Starts recording
            recording = HIGH;
            Serial.print("new file: file_");
            Serial.println(String(fileNum));
            fileNum = fileNum + 1;
        }
    } else if (digitalRead(REC_PIN)==LOW && recording==HIGH) {    // Runs when REC_PIN is LOW (btn unpressed) and recording flag is HIGH (recording)
        // Turns off recording
        recording = LOW;
        Serial.println("end file");
    }
}

//==============

// Reads the value of the joystick for motor control 
// Inputs:  NULL
// Outputs: NULL
void readJoy() {
    joyxValue = analogRead(JOYX_PIN);
}

//==============

// Moves motor A by stepsPerMove steps each time it is called when the joystick is moved from centre value 
// Inputs:  NULL
// Outputs: NULL
void moveA() {
    if (joyxValue > 525) {
        stepperA.step(stepsPerMove);
    } else if (joyxValue < 510) {
        stepperA.step(-stepsPerMove);     
    }
}

//==============

// Moves motor B by stepsPerMove steps each time it is called when the joystick is moved from centre value 
// Inputs:  NULL
// Outputs: NULL
void moveB() {
    if (joyxValue > 525) {
        stepperB.step(stepsPerMove);
    } else if (joyxValue < 510) {
        stepperB.step(-stepsPerMove);      
    }
}

//==============

// Sets all stepper motor A pins to low when not in use 
// Inputs:  NULL
// Outputs: NULL
void idleA() {
  digitalWrite(AIN1_PIN, LOW);
  digitalWrite(AIN2_PIN, LOW);
  digitalWrite(AIN3_PIN, LOW);
  digitalWrite(AIN4_PIN, LOW);
}

//==============

// Sets all stepper motor B pins to low when not in use 
// Inputs:  NULL
// Outputs: NULL

void idleB() {
  digitalWrite(BIN1_PIN, LOW);
  digitalWrite(BIN2_PIN, LOW);
  digitalWrite(BIN3_PIN, LOW);
  digitalWrite(BIN4_PIN, LOW); 
} 
