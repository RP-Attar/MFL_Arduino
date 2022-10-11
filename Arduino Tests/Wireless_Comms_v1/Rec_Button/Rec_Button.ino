// The slave (or the receiver)
// Should be loaded onto the Arduino MEGA to test the record button

#define REC_PIN 40

// Record Button variables
int recState = 0;
long int recStartTime;
int recording = 0; 
int fileNum = 1;

//===========

void setup() {
    // Set up the system
    pinMode(REC_PIN, INPUT);
    Serial.begin(2000000);
}

//=============

void loop() {
    recData();
}

//==============

// Checks if the record button is pressed, if so, sends the signal to send data to a file
// Inputs:  NULL
// Outputs: NULL
void recData() {
    recState = digitalRead(REC_PIN);

    // If we are waiting for the first press, and we press the button
    if (recState==HIGH && recording==0) {
        recStartTime = millis();
        recording = 1;
        Serial.print(recording);
    } 

    // If 5ms has elapsed, and we are still pressing the button
    else if ((millis()-recStartTime)>5 && recording == 1 && recState==HIGH) {
        recording = 2;
        Serial.print(recording);
    }

    else if (recording==2) {
        Serial.print("new file: file_");
        Serial.println(String(fileNum));
        fileNum = fileNum + 1;
        recording = 3;
        Serial.print(recording);
    }

    // If we are recording and we stop pressing the button
    else if (recState==LOW && recording==3) {
        recording = 0;
        Serial.println("end file");
        Serial.print(recording);
    }
}
