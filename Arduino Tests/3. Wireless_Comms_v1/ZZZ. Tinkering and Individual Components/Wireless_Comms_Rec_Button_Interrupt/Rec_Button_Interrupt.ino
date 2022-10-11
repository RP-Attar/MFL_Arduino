// This code hooks the record button up to an interrupt 
// It factors in debounce

#define REC_PIN 2

bool recording = LOW;

long currMillis;
long prevMillis;

void setup() {
    Serial.begin(2000000);
    pinMode(REC_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(REC_PIN), record, CHANGE);
    prevMillis = millis();
}

void loop() {
  currMillis = millis();
  if (currMillis - prevMillis > 1){
    Serial.println(millis());
    prevMillis = currMillis;
  }
}

void record() {
    if (digitalRead(REC_PIN)==HIGH && recording==LOW) {
        delay(2);
        if (digitalRead(REC_PIN)==HIGH) {
          recording = HIGH;
          Serial.println("new_file_");
        }
    } else if (digitalRead(REC_PIN)==LOW && recording==HIGH) {
          recording = LOW;
          Serial.println("end_file");
    }
}
