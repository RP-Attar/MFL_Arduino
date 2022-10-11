// The slave (or the receiver)
// Should be loaded onto the Arduino MEGA

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN  7
#define CSN_PIN 8

// Set the address we are sending info to
const byte thisSlaveAddress[5] = {'R','x','A','A','A'};

// Create a Radio object
RF24 radio(CE_PIN, CSN_PIN);

int dataReceived; // This must match dataToSend in the TX
bool newData = false;

//===========

void setup() {
    // Set up the system
    Serial.begin(9600);
    Serial.println("SimpleTx Starting");

    radio.begin();                                  // Begin the radio
    radio.setDataRate( RF24_1MBPS );                // Set the data rate (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS)
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
        Serial.print("Data received: ");
        Serial.println(dataReceived);
        newData = false;
    }
}
