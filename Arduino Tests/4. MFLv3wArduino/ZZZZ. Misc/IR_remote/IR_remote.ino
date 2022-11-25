#include <IRremote.h>

int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
decode_results results;


void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
}


//Infinite loop
void loop() {
  if (irrecv.decode(&results)) {
    dump(&results);
    irrecv.resume(); // Receive the next value
  }
}

//Dumps the result and prints the numeric received dada and type of remote
void dump(decode_results *results) {
  // Dumps out the decode_results structure.
  // Call this after IRrecv::decode()
  Serial.println(results->value);
  switch (results->value) {
    case 1386468383:
      Serial.println("Prev");
      break;
    case 3622325019:
      Serial.println("Next");
      break;
    case 553536955:
      Serial.println("Play");
      break;
  }
}
