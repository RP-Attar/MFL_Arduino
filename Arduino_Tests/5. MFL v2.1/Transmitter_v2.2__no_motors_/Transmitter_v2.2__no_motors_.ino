// For the Mega

//=========== 
// Libraries to include
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

//=========== 
// Set up struct to store all the data from the Hall Effect Sensors
// Max size of this struct is 32 bytes 
struct Hall_Data {                              
  int comp1 = 0;
  int comp2 = 0;
  int timeDiff = 0;
};

Hall_Data hallData;

//===========
// ADC/Hall Set Up
#define ADC_SCL   21
#define ADC_SDA   20
Adafruit_ADS1115 ads; // use this for the 16-bit version
unsigned long readTime;
unsigned long prevReadTime = 0;

//===========
// Timing set up
unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 200; // send 5x per second

//===========
void setup() {
    // Start up serial comms
    Serial.begin(115200);
    while (!Serial) {}

    // Start up the ADC
    ads.setDataRate(RATE_ADS1115_860SPS);     // fastest
    ads.setGain(GAIN_TWOTHIRDS);                    // +/- 6.144V  1 bit = 0.1875mV (default)
    if (!ads.begin()) {
      Serial.println("Failed to initialize ADS.");
      while (1);
    }
}

void loop() {
    // Gathering data and sending
    currentMillis = millis();
    if (currentMillis - prevMillis >= txIntervalMillis) {
        makeData();
        //sendDataSlow();
        sendDataFast();
        prevMillis = currentMillis;
    }
}


//==============
// Makes the message that will be printed to the serial monitor using sendDataSlow() 
// 'data' is a global struct
void makeData() {
    readTime = micros();
    hallData.comp1 = ads.readADC_Differential_0_1();
    //hallData.comp2 = ads.readADC_Differential_2_3();
    //hallData.comp1 = ads.readADC_SingleEnded(0);
    //hallData.comp2 = ads.readADC_SingleEnded(1);
    hallData.timeDiff = readTime - prevReadTime;
    prevReadTime = readTime;
}

//==============
// Sends the data made using 'makeData()' by displaying it in the serial monitor 
// 'data' is a global struct
void sendDataSlow() {
    Serial.println("Data sent: ");
    Serial.print("comp1: ");
    Serial.println(hallData.comp1);
    //Serial.print("comp2: ");
    //Serial.println(hallData.comp2);
    Serial.print("timeDiff: ");
    Serial.println(hallData.timeDiff);
    Serial.println();
}

void sendDataFast() {
    Serial.println(hallData.comp1);
    //Serial.print(",");
    //Serial.println(hallData.comp2);
}
