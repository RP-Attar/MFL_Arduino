/* The purpose of this code is to read two Hall Effect Sensor wands
 * 
 * This code is owned by ATTAR and was written by Nicholas Lamanna. 
 * Last modified 15/08/2022 by Nicholas Lamanna
 */

// Defining required analog input pins
int HALL1_PIN = A0;
int HALL2_PIN = A1;

int hall_1;
int hall_2;

void setup()
{
  // Initialize the serial port:
  Serial.begin(115200);
}

void loop()
{
  // Reading analog inputs
  hall_1 = analogRead(HALL1_PIN); // 0 -> 1023  
  hall_2 = analogRead(HALL2_PIN); // 0 -> 1023     
 
  Serial.print(hall_1);    
  Serial.print(", ");
  Serial.println(hall_2);      

  delay(2);
}
