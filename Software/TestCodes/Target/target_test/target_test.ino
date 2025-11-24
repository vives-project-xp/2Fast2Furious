// Simple Arduino Nano sketch for a laser receiver + LED indicator
// - When the receiver detects the laser beam, the LED turns on
// - Supports either a digital (HIGH/LOW) photodiode/receiver or an analog sensor
// - Includes simple debounce and Serial calibration output

// Wiring suggestion:
// Laser receiver output -> D2 (or A0 for analog)
// Status LED -> D13 (on-board LED) or external LED with resistor

// Choose input mode: set to true if your receiver gives an analog voltage
// (connect to A0). Set to false for a digital output (connect to D2).
const bool useAnalog = false;

// Pins
const int digitalPinIn = 2; // interrupt-capable pin on Nano
const int analogPinIn = A0;
const int ledPin = 13; // on-board LED

// Threshold for analog detection (0-1023). You can calibrate via Serial.
int analogThreshold = 600;

// Debounce / hold time (ms) to avoid flicker
const unsigned long holdMillis = 100;
unsigned long lastTriggered = 0;

// Score counter: number of times target was hit
unsigned long score = 0;

// For edge detection so we only count one score per hit event
bool lastHitState = false;

// Cooldown after a scored hit (ms). While in cooldown, further hits won't increment.
const unsigned long scoreCooldownMs = 500;
unsigned long lastScoreTime = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  if (!useAnalog) {
    pinMode(digitalPinIn, INPUT);

  } else {
  }

  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  Serial.println("Laser receiver test - starting");
  Serial.print("Mode: "); Serial.println(useAnalog ? "ANALOG" : "DIGITAL");
  if (useAnalog) Serial.print("Threshold: "); Serial.println(analogThreshold);
}

void loop() {
  bool hit = false;

  if (useAnalog) {
    int v = analogRead(analogPinIn);
    hit = (v >= analogThreshold);

    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 500) {
      lastPrint = millis();
      Serial.print("Analog: "); Serial.println(v);
    }
  } else {
    int val = digitalRead(digitalPinIn);
    hit = (val == HIGH);
  }

  if (hit) {
    lastTriggered = millis();
    digitalWrite(ledPin, HIGH);
  }

  if (hit && !lastHitState) {
    unsigned long now = millis();
    if (now - lastScoreTime >= scoreCooldownMs) {
      score++;
      lastScoreTime = now;
      Serial.print("Hit! Score: "); Serial.println(score);
    } else {
      // ignored due to cooldown
    }
  }
  lastHitState = hit;

  if (millis() - lastTriggered > holdMillis) {
    digitalWrite(ledPin, LOW);
  }

  if (Serial.available()) {
    String s = Serial.readStringUntil('\n');
    s.trim();
    if (s.length() > 0) {
      if (useAnalog) {
        int v = s.toInt();
        if (v > 0 && v <= 1023) {
          analogThreshold = v;
          Serial.print("New threshold: "); Serial.println(analogThreshold);
        } else {
          Serial.println("Enter a value 1-1023 to set analog threshold.");
        }
      } else {
        Serial.println("Digital mode: no threshold to set. Send 'stats' to print state.");
        if (s.equalsIgnoreCase("stats")) {
          Serial.print("Digital pin state: ");
          Serial.println(digitalRead(digitalPinIn));
        }
      }
    }
  }

  delay(10);
}
