// The master (or the transmitter)
// Should be loaded onto the Arduino UNO

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN    7
#define CSN_PIN   8
#define HALL_PIN  A0

// Set the address we are sending info to
const byte slaveAddress[5] = {'R','x','A','A','A'};

// Create a Radio object
RF24 radio(CE_PIN, CSN_PIN); 

// Store the message we are going to send
int dataToSend;

// Time interval to send data
unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 100;       // Set timing of message sending

void setup() {
    // Set up the system
    Serial.begin(9600);
    Serial.println("SimpleTx Starting");

    radio.begin();                          // Begin the radio
    radio.setDataRate( RF24_1MBPS );           // Set the data rate (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS)
    radio.setRetries(0,1);                  // Delay (multiples of 250us - (n+1)*250), Count (amount of retries)
    radio.openWritingPipe(slaveAddress);    // Opens a pipe to write to
}

//====================

void loop() {
    // Calls send() when the specified time to send is reached
    currentMillis = millis();
    if (currentMillis - prevMillis >= txIntervalMillis) {
        send();
        prevMillis = millis();
    }
}

//====================

// Sends the current value on the Hall sensor 
// Inputs:  NULL
// Outputs: NULL
void send() {
    // Create the message to be sent
    dataToSend = analogRead(HALL_PIN);

    // Send the message and check it was received
    bool rslt;
    rslt = radio.write( &dataToSend, sizeof(dataToSend) );  // Used to ensure data is received at the other end
        // Always use sizeof() as it gives the size as the number of bytes.
        // For example if dataToSend was an int sizeof() would correctly return 2

    // Log what was sent and if it was received
    Serial.print("Data Sent - ");
    Serial.print(dataToSend);
    Serial.print(" - ");
    if (rslt) {
        Serial.println("Acknowledge received");
    }
    else {
        Serial.println("Tx failed");
    }
}
