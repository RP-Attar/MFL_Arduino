/*
 * Code description here
 */

int hallSensorPin1 = A0; 
int hallSensorPin2 = A1; 
int hallSensorPin3 = A2; 
int hallSensorValue1 = 0;    // variable to store the value coming from the sensor
int hallSensorValue2 = 0;    // variable to store the value coming from the sensor
int hallSensorValue3 = 0;    // variable to store the value coming from the sensor

void setup() {
  // begin the serial monitor @ 9600 baud
  Serial.begin(9600);
}

void loop() {
  // read the value from the sensor:
  hallSensorValue1 = analogRead(hallSensorPin1);
  hallSensorValue2 = analogRead(hallSensorPin2);
  hallSensorValue3 = analogRead(hallSensorPin3);

  Serial.print(hallSensorValue1);
  Serial.print(",");
  Serial.print(hallSensorValue2);
  Serial.print(",");
  Serial.println(hallSensorValue3);

  delay(10);
}
