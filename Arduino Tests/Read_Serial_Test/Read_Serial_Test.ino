
const int btn = 4;

int btnStatePrev = 0;
int btnState = 0;
int btnOnce = 0;        // when 0, we enter file name code, when 1, we don't
int count = 1;

void setup() {
  // put your setup code here, to run once:
  pinMode(btn, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(String(millis()));

  btnStatePrev = btnState;
  btnState = digitalRead(btn);

  if (btnState==1 && btnStatePrev==1 && btnOnce==0) {
    Serial.print("new file: file_");
    Serial.println(String(count));
    btnOnce = 1;
    count = count + 1;
  } else if (btnState==0 && btnStatePrev==0 && btnOnce==1) {
    btnOnce = 0;
    Serial.println("end file");
  }

  delay(5);
}

// python -m pip install serial
