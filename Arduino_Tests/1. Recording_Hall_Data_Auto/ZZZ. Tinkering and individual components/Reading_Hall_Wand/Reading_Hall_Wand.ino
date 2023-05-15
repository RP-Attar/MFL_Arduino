/* The purpose of this code is to read one Hall Effect Sensor wand
 * 
 * This code is owned by ATTAR and was written by Nicholas Lamanna. 
 * Last modified 15/08/2022 by Nicholas Lamanna
 */

// defining required pins analog input (A0)
int hall_1 = A0;

void setup()
{
 // initialize the serial port:
 Serial.begin(9600);
}

void loop()
{
 //reading potentiometers and using to set speeds
 int hall_1_val = analogRead(A0); // 0 -> 1023     
 
 Serial.println(hall_1_val);     

 delay(5);
}
