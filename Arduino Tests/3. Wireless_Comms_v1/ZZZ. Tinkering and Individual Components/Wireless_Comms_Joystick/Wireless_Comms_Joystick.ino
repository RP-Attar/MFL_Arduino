// Set up joystick
#define JOYX_PIN A0
int joyxValue;

void setup() {
  Serial.begin(9600);
}
 
void loop() {
  // Put your main code here, to run repeatedly:
  joyxValue = analogRead(JOYX_PIN);
 
  // Print the values 
  Serial.println(joyxValue);
  delay(100);
}
