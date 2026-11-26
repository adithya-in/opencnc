// S-curve ramp test — all axes, slow start, smooth acceleration.
//
// Velocity follows smoothstep: 0 -> V_MAX over the first 30 % of the move,
// cruise for 40 %, then ease back down over the last 30 %. That gives a
// jerk-limited (S-curve) profile instead of an instant start.
//
// X: STEP D2, DIR D5
// Y: STEP D3, DIR D6
// Z: STEP D4, DIR D7
// EN: D8 (active LOW)

const int EN = 8;
const int MOTORS = 3;
const int stepPin[MOTORS] = {2, 3, 4};
const int dirPin[MOTORS]  = {5, 6, 7};

const int   N        = 3200;    // steps per move = 1 rev at 1/16
const float V_START  = 200.0;   // steps/s  (2.5 mm/s)
const float V_MAX    = 3000.0;  // steps/s  (37 mm/s)
const int   PULSE_US = 5;

float smoothstep(float t) {
  if (t < 0) t = 0;
  if (t > 1) t = 1;
  return t * t * (3.0 - 2.0 * t);
}

void moveAll(bool forward) {
  for (int m = 0; m < MOTORS; m++) digitalWrite(dirPin[m], forward ? HIGH : LOW);
  delayMicroseconds(20);

  unsigned long next = micros();

  for (int i = 0; i < N; i++) {
    float p = (float)i / (float)(N - 1);
    float f;
    if (p < 0.3)       f = smoothstep(p / 0.3);
    else if (p < 0.7)  f = 1.0;
    else               f = smoothstep((1.0 - p) / 0.3);

    float v = V_START + (V_MAX - V_START) * f;
    unsigned long interval = (unsigned long)(1000000.0 / v);

    while ((long)(micros() - next) < 0) { }

    for (int m = 0; m < MOTORS; m++) digitalWrite(stepPin[m], HIGH);
    delayMicroseconds(PULSE_US);
    for (int m = 0; m < MOTORS; m++) digitalWrite(stepPin[m], LOW);

    next += interval;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(EN, OUTPUT);
  digitalWrite(EN, LOW);

  for (int m = 0; m < MOTORS; m++) {
    pinMode(stepPin[m], OUTPUT);
    pinMode(dirPin[m], OUTPUT);
    digitalWrite(stepPin[m], LOW);
    digitalWrite(dirPin[m], LOW);
  }

  Serial.println();
  Serial.println("S-curve ramp: 200 -> 3000 steps/s (2.5 -> 37 mm/s)");
  Serial.println("One revolution each way.");
  delay(1500);
}

void loop() {
  Serial.println("forward  (ramp up, cruise, ramp down)");
  moveAll(true);
  delay(800);

  Serial.println("reverse");
  moveAll(false);
  delay(800);
}
