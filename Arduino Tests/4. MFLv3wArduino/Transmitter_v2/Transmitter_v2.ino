// SimpleRx - the slave or the receiver

//=========== 
// Libraries to include
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


//===========
// Wireless Comms Setup
#define CE_PIN    39
#define CSN_PIN   41
#define IRQ_PIN  3

const byte addresses[][6] = {"00001", "00002"};   // Addresses for write/read

struct Data_Package {                             // Max size of this struct is 32 bytes - NRF24L01 buffer limit
  int comp1 = 1024;
  int comp2 = 1000;
  int timeDiff = 2000;
  int stepDiff = 2;
};

bool newDataSend = false;                         // True when there is new data to send
bool newDataReceived = false;                          // True when there is new data received

RF24 radio(CE_PIN, CSN_PIN);
Data_Package data;
char surpriseMsg[15] = "test";

//===========
// Timing set up
unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 1000; // send once per second

//===========

void setup() {
    // Start up serial monitor 
    Serial.begin(9600);

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
    attachInterrupt(digitalPinToInterrupt(IRQ_PIN), getData, FALLING);
    //radio.printDetails();  
}

//=============

void loop() {
    currentMillis = millis();
    showData();
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
    data.comp1 = random(1000);
    data.comp2 = random(1000);
    data.timeDiff = random(1000);
    data.stepDiff = random(1000);
    newDataSend = true;
}

//==============
// Sends the data made using 'makeData()', and displays it in the serial monitor 
// 'data' is a global struct
void sendDataSlow() {
    if (newDataSend == true) {
      radio.write(&data, sizeof(Data_Package));
      Serial.println("Data sent: ");
      Serial.print("comp1: ");
      Serial.println(data.comp1);
      Serial.print("comp2: ");
      Serial.println(data.comp2);
      Serial.print("timeDiff: ");
      Serial.println(data.timeDiff);
      Serial.print("stepDiff: ");
      Serial.println(data.stepDiff);
      Serial.println();
    }
}

//==============
// Sends the data made using 'makeData()' 
// 'data' is a global struct
void sendDataFast() {
    if (newDataSend == true) {
      radio.write(&data, sizeof(Data_Package));
    }
}

//==============
// Gets the incoming data, when the IRQ pin is triggered (hence the interrupt)
// 'data' is a global struct
void getData() {
    if ( radio.available() ) {
      newDataReceived = true;
    }
}

//==============
// shows the obtained data, if the interupt was triggered since the last display
// 'surpriseMsg' is a global variable
void showData() {
    if (newDataReceived == true) {
        radio.read(&surpriseMsg, sizeof(surpriseMsg)); // Read the whole data and store it into the 'surpriseMsg' variable;
        Serial.println(surpriseMsg);
        newDataReceived = false;
    }
}
