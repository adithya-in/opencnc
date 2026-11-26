// Diagnostic: enable toggle + slow single-step on X.
//
// Phase 1: EN LOW for 5 s  -> drivers enabled, shaft should feel stiff
// Phase 2: 10 single steps on X, 600 ms apart -> should visibly jerk
// Phase 3: EN HIGH for 5 s -> drivers disabled, shaft should spin freely
//
// If stiffness never changes, the driver has no VMOT/logic or is dead.
// If it is stiff but never steps, the problem is Vref, jumpers or wiring.

const int EN     = 8;
const int X_STEP = 2;
const int X_DIR  = 5;

void setup() {
  Serial.begin(115200);
  pinMode(EN, OUTPUT);
  pinMode(X_STEP, OUTPUT);
  pinMode(X_DIR, OUTPUT);
  digitalWrite(X_STEP, LOW);
  digitalWrite(X_DIR, LOW);
  Serial.println();
  Serial.println("DIAG: enable + single step");
  delay(1000);
}

void loop() {
  Serial.println("--- EN LOW (enabled): shaft should be STIFF");
  digitalWrite(EN, LOW);
  delay(5000);

  Serial.println("--- 10 slow steps on X: watch the pulley");
  for (int i = 0; i < 10; i++) {
    digitalWrite(X_STEP, HIGH);
    delayMicroseconds(10);
    digitalWrite(X_STEP, LOW);
    delay(600);
  }

  Serial.println("--- EN HIGH (disabled): shaft should spin FREE");
  digitalWrite(EN, HIGH);
  delay(5000);
}
