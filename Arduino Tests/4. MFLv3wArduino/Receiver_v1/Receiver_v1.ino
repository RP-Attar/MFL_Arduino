// Rx - the slave or the receiver

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

struct Hall_Data {      // Max size of this struct is 32 bytes - NRF24L01 buffer limit
  int comp1 = 1024;
  int comp2 = 1000;
  int timeDiff = 2000;
  int stepDiff = 2;
};

struct Instructions {   // Max size of this struct is 32 bytes - NRF24L01 buffer limit
  char dir = 'L';
  bool rec = false;
  bool clippedOn = false;
};

bool newDataSend = false;                         // True when there is new data to send
bool newDataReceived = false;                     // True when there is new data received

RF24 radio(CE_PIN, CSN_PIN);
Hall_Data hallData;
Instructions surpriseMsg;

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
      makeSurprise();
      surprise();
      Serial.println("surprise sent");
    }
}

//==============
// Gets the incoming data, if there is any available
// 'data' is a global struct
void getData() {
    if ( radio.available() ) {
        radio.read(&hallData, sizeof(Hall_Data)); // Read the data and store it into the 'hallData' structure;
        newDataReceived = true;
    }
}

//==============
// Displays the 'data' struct that was updated using 'getData()'
// 'data' is a global struct
void showData() {
    if (newDataReceived == true) {
        Serial.println("Data received: ");
        Serial.print("comp1: ");
        Serial.println(hallData.comp1);
        Serial.print("comp2: ");
        Serial.println(hallData.comp2);
        Serial.print("timeDiff: ");
        Serial.println(hallData.timeDiff);
        Serial.print("stepDiff: ");
        Serial.println(hallData.stepDiff);
        Serial.println();
        newDataReceived = false;
    }
}

//==============
// Makes a surprise message when called
void makeSurprise() {
    surpriseMsg.dir = 'R';
    surpriseMsg.rec = false;
    surpriseMsg.clippedOn = true;
    newDataSend = true;
}

//==============
// Sends a surprise message when called
void surprise() {
    if (newDataSend == true) {
      radio.stopListening();
      radio.write(&surpriseMsg, sizeof(surpriseMsg));
      newDataSend = false;
      radio.startListening();
    }
}
