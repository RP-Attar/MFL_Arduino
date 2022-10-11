/* The purpose of this code is to control the DC motors in the Bearing Demo apparatus.
 * 
 * This code is owned by ATTAR and was written by Reuben Praveen. 
 * Last modified 15/08/2022 by Nicholas Lamanna
 */

// defining required pins for motor control (10-13) and potentiometer inputs (5-6)
int in1=13;
int in2=12;
int in3=11;
int in4=10;
int speedPinA=6;
int speedPinB=5;

void setup()
{
 pinMode(in1,OUTPUT);
 pinMode(in2,OUTPUT);
 pinMode(in3,OUTPUT);
 pinMode(in4,OUTPUT);

 digitalWrite(in1,LOW); 
 digitalWrite(in2,LOW); 
 digitalWrite(in3,LOW); 
 digitalWrite(in4,LOW); 

 // initialize the serial port:
 Serial.begin(9600);
}

void loop()
{
 //reading potentiometers and using to set speeds
 int n1 = analogRead(A0); // 0 -> 1023     
 int n2 = analogRead(A1); // 0 -> 1023
 
 Serial.print(n1);
 Serial.print(", ");
 Serial.println(n2);     

 delay(10);
}
