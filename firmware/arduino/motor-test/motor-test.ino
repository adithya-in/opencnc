// Minimal CNC Shield motor test — bypasses GRBL completely.
//
// Flashing this replaces GRBL. Reflash grbl-pen-servo afterwards.
//
// CNC Shield V3 pins:
//   X STEP D2   X DIR D5
//   Y STEP D3   Y DIR D6
//   Enable D8 (active LOW)
//
// At 1/16 microstepping with 20T GT2 (80 steps/mm), 800 steps = 10 mm.

const int EN     = 8;
const int X_STEP = 2, X_DIR = 5;
const int Y_STEP = 3, Y_DIR = 6;

const int STEPS = 800;      // 10 mm at 80 steps/mm
const int STEP_US = 5;      // pulse width
const int GAP_US  = 1200;   // time between pulses (~830 steps/s)

void setup() {
  Serial.begin(115200);

  pinMode(EN, OUTPUT);
  digitalWrite(EN, LOW);          // LOW = drivers enabled

  pinMode(X_STEP, OUTPUT); pinMode(X_DIR, OUTPUT);
  pinMode(Y_STEP, OUTPUT); pinMode(Y_DIR, OUTPUT);
  digitalWrite(X_STEP, LOW); digitalWrite(Y_STEP, LOW);

  Serial.println();
  Serial.println("CNC Shield motor test");
  Serial.println("If the motors hold torque but do not move, check the");
  Serial.println("microstep jumpers and the 12 V motor supply.");
  delay(1500);
}

void move(int stepPin, int dirPin, bool dir, int n) {
  digitalWrite(dirPin, dir ? HIGH : LOW);
  delayMicroseconds(10);
  for (int i = 0; i < n; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(STEP_US);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(GAP_US);
  }
}

void loop() {
  Serial.println("X forward");
  move(X_STEP, X_DIR, HIGH, STEPS);
  delay(600);

  Serial.println("X back");
  move(X_STEP, X_DIR, LOW, STEPS);
  delay(600);

  Serial.println("Y forward");
  move(Y_STEP, Y_DIR, HIGH, STEPS);
  delay(600);

  Serial.println("Y back");
  move(Y_STEP, Y_DIR, LOW, STEPS);
  delay(2000);
}
