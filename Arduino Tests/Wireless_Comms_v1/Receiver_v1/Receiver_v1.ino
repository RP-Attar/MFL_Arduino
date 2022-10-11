// The slave (or the receiver)
// Should be loaded onto the Arduino MEGA

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN  39
#define CSN_PIN 41
#define REC_PIN 2

// Set the address we are sending info to
const byte thisSlaveAddress[5] = {'R','x','A','A','A'};

// Create a Radio object and variables for receiving 
RF24 radio(CE_PIN, CSN_PIN);
char dataReceived[32]; // This must match dataToSend in the TX
bool newData = false;

// Record button variables
int fileNum = 1;
bool recording = LOW;

//===========

void setup() {
    // Set up the system
    Serial.begin(2000000);                          // Make sure this is the same accross all programs
    Serial.println("SimpleTx Starting");

    // Start up the recording button interrupt
    pinMode(REC_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(REC_PIN), recData, CHANGE);

    // Begin and set up the radio obeject 
    radio.begin();                                  // Begin the radio
    radio.setDataRate( RF24_2MBPS );                // Set the data rate (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS)
    radio.openReadingPipe(1, thisSlaveAddress);     // Opens a pipe to read from 
    radio.startListening();                         // Begin listening for messages
}

//=============

void loop() {
    getData();
    showData();
}

//==============

// Checks if there is data being sent, stores it, and sets newData flag to true
// Inputs:  NULL
// Outputs: NULL
void getData() {
    if ( radio.available() ) {
        radio.read( &dataReceived, sizeof(dataReceived) );
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
