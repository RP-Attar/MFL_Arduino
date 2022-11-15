// SimpleRx - the slave or the receiver

//=========== 
// Libraries to include
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


//===========
// Wireless Comms Setup
#define CE_PIN  9
#define CSN_PIN 10

const byte addresses[][6] = {"00001", "00002"};   // Addresses for read/write

struct Data_Package {   // Max size of this struct is 32 bytes - NRF24L01 buffer limit
  int comp1 = 1024;
  int comp2 = 1000;
  int timeDiff = 2000;
  int stepDiff = 2;
};

bool newData = false;   // True when there is new data to receive

RF24 radio(CE_PIN, CSN_PIN);
Data_Package data;
char surpriseMsg[15] = "Hello";

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
    radio.openWritingPipe(addresses[1]);    // Opens a pipe to write to
    radio.openReadingPipe(0, addresses[0]); // Opens a pipe to read from
    radio.startListening();
    //radio.printDetails();  
}

//=============

void loop() {
    getData();
    showData();
    if (random()%14020==0){
      surprise();
      Serial.println("surprise sent");
    }
}

//==============
// Gets the incoming data, if there is any available
// 'data' is a global struct
void getData() {
    if ( radio.available() ) {
        radio.read(&data, sizeof(Data_Package)); // Read the whole data and store it into the 'data' structure;
        newData = true;
    }
}

//==============
// Displays the 'data' struct that was updated using 'getData()'
// 'data' is a global struct
void showData() {
    if (newData == true) {
        Serial.println("Data received: ");
        Serial.print("comp1: ");
        Serial.println(data.comp1);
        Serial.print("comp2: ");
        Serial.println(data.comp2);
        Serial.print("timeDiff: ");
        Serial.println(data.timeDiff);
        Serial.print("stepDiff: ");
        Serial.println(data.stepDiff);
        Serial.println();
        newData = false;
    }
}

//==============
// Sends a surprise message when called
void surprise() {
    radio.stopListening();
    radio.write( &surpriseMsg, sizeof(surpriseMsg) );
    radio.startListening();
}
