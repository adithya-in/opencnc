// Spin all CNC Shield motors continuously.
//
// X: STEP D2, DIR D5
// Y: STEP D3, DIR D6
// Z: STEP D4, DIR D7
// Enable: D8 (active LOW)
//
// One revolution each way, forever. At 1/16 microstepping a NEMA17 is
// 3200 steps/rev; with an 800 us gap that is ~2.6 s per revolution.

const int EN = 8;

const int MOTORS = 3;
const int stepPin[MOTORS] = {2, 3, 4};   // X, Y, Z
const int dirPin[MOTORS]  = {5, 6, 7};

const int STEPS_PER_REV = 3200;
const int STEP_US = 5;
const int GAP_US  = 800;

void spin(bool forward, int n) {
  for (int m = 0; m < MOTORS; m++) digitalWrite(dirPin[m], forward ? HIGH : LOW);
  delayMicroseconds(20);

  for (int s = 0; s < n; s++) {
    for (int m = 0; m < MOTORS; m++) digitalWrite(stepPin[m], HIGH);
    delayMicroseconds(STEP_US);
    for (int m = 0; m < MOTORS; m++) digitalWrite(stepPin[m], LOW);
    delayMicroseconds(GAP_US);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(EN, OUTPUT);
  digitalWrite(EN, LOW);                 // enable all drivers

  for (int m = 0; m < MOTORS; m++) {
    pinMode(stepPin[m], OUTPUT);
    pinMode(dirPin[m], OUTPUT);
    digitalWrite(stepPin[m], LOW);
    digitalWrite(dirPin[m], LOW);
  }

  Serial.println();
  Serial.println("All motors spinning: X, Y, Z");
  Serial.println("3200 steps per revolution, ~2.6 s each way");
  delay(1500);
}

void loop() {
  Serial.println("forward");
  spin(true, STEPS_PER_REV);
  delay(500);

  Serial.println("reverse");
  spin(false, STEPS_PER_REV);
  delay(500);
}
