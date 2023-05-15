// The slave (or the receiver)
// Should be loaded onto the Arduino MEGA
// Ensure string is tight before pressing record, otherwise measurements will be off

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
#define IRQ_PIN 3     // nRF24 - interrupt pin
// Set the address we are sending info to
const byte thisSlaveAddress[5] = {'R','x','A','A','A'};
// Create a Radio object and variables for receiving 
RF24 radio(CE_PIN, CSN_PIN);
char dataReceived[32]; // This must match dataToSend in the TX

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
const int stepperSpeed = 80;
int stepsSinceLastRead = 0;
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

    // Start up the radio obeject and IRQ pin interrupt 
    if (!radio.begin()) {                   // Begin the radio
        Serial.println(F("radio hardware not responding!"));
        while (1) {} // hold program in infinite loop to prevent subsequent errors
    }   
    radio.setDataRate( RF24_2MBPS );                // Set the data rate (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS)
    radio.openReadingPipe(1, thisSlaveAddress);     // Opens a pipe to read from 
    radio.startListening();                         // Begin listening for messages
    radio.maskIRQ(1, 1, 0);                         // Means that only "data received" will trigger interrupt on falling edge
    pinMode(IRQ_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(IRQ_PIN), getData, FALLING);
    
    // Start up the stepper motors at X RPM
    stepperA.setSpeed(stepperSpeed);
    stepperB.setSpeed(stepperSpeed);
}

//=============

// Runs forever
void loop() {
    recData();
    readJoy();
    moveA();
    //idleA();
    moveB();
    //idleB();
}

//==============

// Triggered by ISR interrupt on radio transmitter of receiver to show successfully received data
// Inputs:  NULL
// Outputs: NULL
void getData() {
    bool tx_ds, tx_df, rx_dr;
    radio.whatHappened(tx_ds, tx_df, rx_dr); // resets the IRQ pin to HIGH (to make .available reliable)
    if (radio.available()) {
        radio.read(&dataReceived, sizeof(dataReceived));
        Serial.print(stepsSinceLastRead);
        Serial.print(",");
        Serial.println(dataReceived);
        stepsSinceLastRead = 0;
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
        stepperA.step(-stepsPerMove);
    } else if (joyxValue < 510) {
        stepperA.step(stepsPerMove);     
    }
}

//==============

// Moves motor B by stepsPerMove steps each time it is called when the joystick is moved from centre value 
// Inputs:  NULL
// Outputs: NULL
void moveB() {
    if (joyxValue > 525) {
        stepperB.step(-stepsPerMove);
        stepsSinceLastRead = stepsSinceLastRead + stepsPerMove;
    } else if (joyxValue < 510) {
        stepperB.step(stepsPerMove);      
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
