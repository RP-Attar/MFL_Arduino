// The master (or the transmitter)
// Should be loaded onto the Arduino UNO

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
#include <stdio.h>

#define CE_PIN      9
#define CSN_PIN     10
#define HALL1_PIN   A0
#define HALL2_PIN   A1

// Set the address we are sending info to
const byte slaveAddress[5] = {'R','x','A','A','A'};

// Create a Radio object
RF24 radio(CE_PIN, CSN_PIN); 

// Store the message we are going to send
char dataToSend[32];
int hall1;
int hall2;

// Time interval to send data (must uncomment some of the code in 'loop()')
unsigned long currentMicros;
unsigned long prevMicros = 0;
int diffMicros;
//unsigned long txIntervalMilli = 2000;       // Set timing of message sending

void setup() {
    // Set up the system
    Serial.begin(2000000);
    printf_begin();
    Serial.println("SimpleTx Starting");

    radio.begin();                          // Begin the radio
    radio.printDetails();                   // Allows you to check connection from board to chip is ok
    radio.setDataRate( RF24_2MBPS );        // Set the data rate (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS)
    radio.setRetries(0,1);                  // Delay (multiples of 250us - (n+1)*250), Count (amount of retries)
    radio.openWritingPipe(slaveAddress);    // Opens a pipe to write to
}

//====================

void loop() {
    // Runs when the specified time to send is reached
    //currentMicro = micros();
    //if (currentMicro - prevMicro >= txIntervalMicro) {
        readHall();
        updateData();
        send();
        //prevMicro = micros();
    //}
}

//====================

// Sends the most recently created dataToSend
// Inputs:  NULL
// Outputs: NULL
void send() {
    // Send the message and check it was received
    bool rslt;
    rslt = radio.write( &dataToSend, sizeof(dataToSend) );  // Used to ensure data is received at the other end
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
    radio.write( &dataToSend, sizeof(dataToSend) );  // Used to ensure data is received at the other end
        // Always use sizeof() as it gives the size as the number of bytes.
        // For example if dataToSend was an int sizeof() would correctly return 2
}

//====================

// Updates the message to be sent 
// Inputs:  NULL
// Outputs: NULL
void updateData() {
    snprintf(dataToSend, sizeof(dataToSend), "%d,%d,%d", diffMicros, hall1, hall2);
}

//====================

// Reads the Hall effect sensors and sets the time of the read
// Inputs:  NULL
// Outputs: NULL
void readHall() {
    currentMicros = micros();
    hall1 = analogRead(HALL1_PIN);
    hall2 = analogRead(HALL2_PIN);
    diffMicros = currentMicros - prevMicros;
    prevMicros = currentMicros;
}
