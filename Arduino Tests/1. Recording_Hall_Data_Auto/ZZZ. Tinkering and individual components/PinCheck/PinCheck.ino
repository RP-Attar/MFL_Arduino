#define LED 13              // The pin the LED is connected to

void setup() {
  pinMode(LED, OUTPUT);     // Declare the LED as an output
}

void loop() {
  digitalWrite(LED, HIGH);  // Turn the LED on
  delay(1000);
  digitalWrite(LED, LOW);   // Turn the LED off
  delay(1000);
}
