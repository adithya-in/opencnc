// Hold the stepper enable LOW permanently, and tick X slowly.
//
// Use this to measure EN at the driver without timing ambiguity:
//   EN at the driver should read ~0 V at all times.
//   If it reads ~4.6 V, the Arduino is not connected to that pin.

const int EN     = 8;
const int X_STEP = 2;
const int X_DIR  = 5;

void setup() {
  Serial.begin(115200);
  pinMode(EN, OUTPUT);
  digitalWrite(EN, LOW);           // enable, permanently

  pinMode(X_STEP, OUTPUT);
  pinMode(X_DIR, OUTPUT);
  digitalWrite(X_STEP, LOW);
  digitalWrite(X_DIR, LOW);

  Serial.println();
  Serial.println("EN held LOW permanently. Measure EN at the driver:");
  Serial.println("  ~0 V  = enable line is connected");
  Serial.println("  ~4.6 V = enable line is open / not driven");
  delay(1000);
}

void loop() {
  // one slow step every 1.5 s so the pulley visibly ticks
  digitalWrite(X_STEP, HIGH);
  delayMicroseconds(10);
  digitalWrite(X_STEP, LOW);
  delay(1500);
}
