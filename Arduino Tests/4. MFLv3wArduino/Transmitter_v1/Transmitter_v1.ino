// The device (or the transmitter)
// Should be loaded onto the Arduino MEGA

//===========

// Libraries to include
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
#include <stdio.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <AccelStepper.h>

//===========

// Wireless Set Up
#define CE_PIN      9                                 // Connect to any pin you like on Mega
#define CSN_PIN     10                                // Connect to any pin you like on Mega
// Set the address we are sending info to
const byte slaveAddress[5] = {'R','x','A','A','A'};   // Must match address on the receiving device (if we write to this address) 
// Create a Radio object
RF24 radio(CE_PIN, CSN_PIN); 
// Store the message we are going to send
char dataToSend[32];

//===========

// ADC/Hall Set Up
#define ADC_SCL   20                                  // Check this is the correct pin for I2C on Mega  
#define ADC_SDA   21                                  // Check this is the correct pin for I2C on Mega
Adafruit_ADS1115 ads;                                 // Use this for the 16-bit version
int comparison1;                                      // Stores comparison between chan0 and chan1
int comparison2;                                      // Stores comparison between chan2 and chan3
unsigned long readTime;                               // This and the next 3 variables are used for timing the reading of the ADC
unsigned long prevReadTime = 0;
int readTimeDiff;

//===========

// Timing Set Up (consider using ALRT pin on ADS1115 instead, if we upgrade system to run faster than the Mega)
unsigned long currentMicros;
unsigned long prevMicros = 0;
int txWaitTime = 1000;

//===========

// Stepper motors setup
// Motor interface type must be set to 1 when using a driver:
#define stepPinA 6
#define dirPinA 7
#define stepPinB 8
#define dirPinB 9
#define motorInterfaceType 1
const int stepsPerRevolution = 200;
const int stepsPerMove = 1;
int motorSpeed = 380;
int stepsSinceLastRead = 0;
// Create a new instance of the AccelStepper class:
AccelStepper stepperA = AccelStepper(motorInterfaceType, stepPinA, dirPinA);
AccelStepper stepperB = AccelStepper(motorInterfaceType, stepPinB, dirPinB);

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
    radio.setDataRate(RF24_2MBPS);          // Set the data rate (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS)
    radio.setRetries(1,4);                  // Delay (multiples of 250us - (n+1)*250), Count (amount of retries)
    radio.openWritingPipe(slaveAddress);    // Opens a pipe to write to
    radio.printDetails();                   // Allows you to check connection from board to chip is ok

    // Start up the ADC
    ads.setDataRate(RATE_ADS1115_860SPS);   // fastest
    ads.setGain(GAIN_TWOTHIRDS);            //+/- 6.144V  1 bit = 0.1875mV (default)
    if (!ads.begin()) {
      Serial.println("Failed to initialize ADS.");
      while (1);
    }
}

//====================
//====================
//====================
//====================
//====================

void loop() {
    currentMicros = micros();    
    
    if(currentMicros-prevMicros>txWaitTime) {
        getMsg();
        sendSlow();
        prevMicros = currentMicros;      
    }
}

//====================
//====================
//====================
//====================
//====================

// Sends the most recently created dataToSend
// Inputs:  NULL
// Outputs: NULL
void sendSlow() {
    // Send the message and check it was received
    bool rslt;
    rslt = radio.write(&dataToSend, sizeof(dataToSend));  
        // Used to ensure data is received at the other end
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
    radio.write(&dataToSend, sizeof(dataToSend));  
        // Used to ensure data is received at the other end
        // Always use sizeof() as it gives the size as the number of bytes.
        // For example if dataToSend was an int sizeof() would correctly return 2
}

//====================
  
// Commpares the Hall effect sensors and creates the message
// Inputs:  NULL
// Outputs: NULL
void getMsg() {
    readTime = micros();                            // Sets the time for the next readins of the ADC
    comparison1 = ads.readADC_Differential_0_1();   // Compares ADC chan0 to chan1 and spits out the difference
    comparison2 = ads.readADC_Differential_2_3();   // Compares ADC chan2 to chan3 and spits out the difference
    readTimeDiff = readTime - prevReadTime;         // Records the time it took between the last reading, and this reading
    snprintf(dataToSend, sizeof(dataToSend), "%d,%d,%d", readTimeDiff, comparison1, comparison2); // Outputs all the data from the last few lines into an array of bytes
    prevReadTime = readTime;                        // Resets the timer so we know how long to the next read
}
