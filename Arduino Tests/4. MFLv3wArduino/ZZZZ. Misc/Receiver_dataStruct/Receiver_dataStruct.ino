#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";

// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct Data_Package {
  byte a = 0;
  byte b = 125;
  byte c = 255;
  int d = 1024;
  float e = 3.141592;
  String f = "Test";
};

Data_Package data; //Create a variable with the above structure

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  // Check whether there is data to be received
  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package)); // Read the whole data and store it into the 'data' structure
  }
  Serial.print("a: ");
  Serial.print(data.a);
  Serial.print(" b: ");
  Serial.print(data.b);
  Serial.print(" c: ");
  Serial.print(data.c);
  Serial.print(" d: ");
  Serial.print(data.d);
  Serial.print(" e: ");
  Serial.print(data.e);
  Serial.print(" f: ");
  Serial.println(data.f);
}
