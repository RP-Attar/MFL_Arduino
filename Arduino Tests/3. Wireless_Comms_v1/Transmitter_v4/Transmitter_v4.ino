// The master (or the transmitter)
// Should be loaded onto the Arduino UNO

//===========

// Libraries to include
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
#include <stdio.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

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

// ADC/Hall Set Up
#define ADC_SCL   A5
#define ADC_SDA   A4
Adafruit_ADS1115 ads;               // Use this for the 16-bit version
const float multiplier = 0.1875F;
int hall1;
int hall2;
int result;
unsigned long readTime;
unsigned long prevReadTime = 0;
int readDiff;

//===========

// Timing Set Up
unsigned long currentMicros;
unsigned long prevMicros = 0;
int txWaitTime = 1000;

//===========

void setup() {
    // Set up Serial comms
    Serial.begin(2000000);
    printf_begin();
    Serial.println("SimpleTx Starting");

    // I2C comms set up
    Wire.setClock(400000);

    // Start up the radio obeject
    if (!radio.begin()) {                   // Begin the radio
        Serial.println(F("radio hardware not responding!"));
        while (1) {} // hold program in infinite loop to prevent subsequent errors
    }                          
    radio.setDataRate( RF24_2MBPS );        // Set the data rate (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS)
    radio.setRetries(1,4);                  // Delay (multiples of 250us - (n+1)*250), Count (amount of retries)
    radio.openWritingPipe(slaveAddress);    // Opens a pipe to write to
    radio.printDetails();                   // Allows you to check connection from board to chip is ok

    // Start up the ADC
    ads.setDataRate(RATE_ADS1115_860SPS);   // fastest
    ads.setGain(GAIN_TWOTHIRDS);            //+/- 6.144V  1 bit = 0.1875mV (default)
    // ads.setGain(GAIN_ONE);               //+/- 4.096V  1 bit = 0.125mV
    // ads.setGain(GAIN_TWO);               //+/- 2.048V  1 bit = 0.0625mV
    // ads.setGain(GAIN_FOUR);              //+/- 1.024V  1 bit = 0.03125mV
    // ads.setGain(GAIN_EIGHT);             //+/- 0.512V  1 bit = 0.015625mV
    // ads.setGain(GAIN_SIXTEEN);           //+/- 0.256V  1 bit = 0.0078125mV 
    // ads.startADCReading(0, false);
    if (!ads.begin()) {
      Serial.println("Failed to initialize ADS.");
      while (1);
    }
}

//====================

void loop() {
    currentMicros = micros();    
    
    if(currentMicros-prevMicros>txWaitTime) {
        getMsg();
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
  
// Commpares the Hall effect sensors and creates the message
// Inputs:  NULL
// Outputs: NULL
void getMsg() {
    readTime = micros();
    result = ads.readADC_Differential_0_1();
    readDiff = readTime - prevReadTime;
    snprintf(dataToSend, sizeof(dataToSend), "%d,%d", readDiff, result);
    prevReadTime = readTime;
}
