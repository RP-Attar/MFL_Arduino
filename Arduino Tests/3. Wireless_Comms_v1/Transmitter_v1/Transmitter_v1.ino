// The master (or the transmitter)
// Should be loaded onto the Arduino UNO

//===========

// Libraries to include
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
#include <stdio.h>

//===========

// Wireless Set Up
#define CE_PIN      9
#define CSN_PIN     10
// Set the address we are sending info to
const byte slaveAddress[5] = {'R','x','A','A','A'};
// Create a Radio object
RF24 radio(CE_PIN, CSN_PIN); 
// Store the message we are going to send
char dataToSend[32];

//===========

// Hall Set Up
#define HALL1_PIN   A0
#define HALL2_PIN   A1
int hall1;
int hall2;
unsigned long readTime;
unsigned long prevReadTime = 0;
int readDiff;

//===========

// Timing Set Up
unsigned long currentMicros;
unsigned long prevMicros = 0;
int txWaitTime = 5000;

//===========

void setup() {
    // Set up Serial comms
    Serial.begin(2000000);
    printf_begin();
    Serial.println("SimpleTx Starting");

    // Start up the radio obeject
    radio.begin();                          // Begin the radio
    radio.setDataRate( RF24_2MBPS );        // Set the data rate (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS)
    radio.setRetries(1,4);                  // Delay (multiples of 250us - (n+1)*250), Count (amount of retries)
    radio.openWritingPipe(slaveAddress);    // Opens a pipe to write to
    radio.printDetails();                   // Allows you to check connection from board to chip is ok
}

//====================

void loop() {
    currentMicros = micros();    
    
    if(currentMicros-prevMicros>txWaitTime) {
        readHall();
        updateData();
        send();
        prevMicros = currentMicros;      
    }
}

//====================

// Sends the most recently created dataToSend
// Inputs:  NULL
// Outputs: NULL
void send() {
    // Send the message and check it was received
    bool rslt;
    rslt = radio.write(&dataToSend, sizeof(dataToSend));  // Used to ensure data is received at the other end
        // Always use sizeof() as it gives the size as the number of bytes.
        // For example if dataToSend was an int sizeof() would correctly return 2

    // Log what was sent and if it was received
    Serial.print("Time: ");
    Serial.print(micros());
    Serial.print(", Data Sent: ");
    Serial.print(dataToSend);
    Serial.print(" - ");
    if (rslt) {
        Serial.println("Ack Received");
    }
    else {
        Serial.println("Tx Failed");
    }
}

//====================

// Sends the most recently created dataToSend (but without all the printing)
// Inputs:  NULL
// Outputs: NULL
void sendQuick() {
    // Send the message and check it was received
    radio.write(&dataToSend, sizeof(dataToSend));  // Used to ensure data is received at the other end
        // Always use sizeof() as it gives the size as the number of bytes.
        // For example if dataToSend was an int sizeof() would correctly return 2
}

//====================

// Updates the message to be sent 
// Inputs:  NULL
// Outputs: NULL
void updateData() {
    snprintf(dataToSend, sizeof(dataToSend), "%d,%d,%d", readDiff, hall1, hall2);
}

//====================

// Reads the Hall effect sensors
// Inputs:  NULL
// Outputs: NULL
void readHall() {
    readTime = micros();
    hall1 = analogRead(HALL1_PIN);
    hall2 = analogRead(HALL2_PIN);
    readDiff = readTime - prevReadTime;
    prevReadTime = readTime;
}
