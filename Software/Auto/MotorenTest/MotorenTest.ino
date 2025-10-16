// Motor 1
#define M1_IN1 2
#define M1_IN2 3
#define M1_EN  4

// Motor 2
#define M2_IN1 5
#define M2_IN2 6
#define M2_EN  9

// Motor 3
#define M3_IN1 10
#define M3_IN2 11
#define M3_EN  12

// Motor 4
#define M4_IN1 13
#define M4_IN2 8
#define M4_EN  A0

void setup() {
  pinMode(M1_IN1, OUTPUT);
  pinMode(M1_IN2, OUTPUT);
  pinMode(M1_EN, OUTPUT);

  pinMode(M2_IN1, OUTPUT);
  pinMode(M2_IN2, OUTPUT);
  pinMode(M2_EN, OUTPUT);

  pinMode(M3_IN1, OUTPUT);
  pinMode(M3_IN2, OUTPUT);
  pinMode(M3_EN, OUTPUT);

  pinMode(M4_IN1, OUTPUT);
  pinMode(M4_IN2, OUTPUT);
  pinMode(M4_EN, OUTPUT);

  Serial.begin(9600);
}

// Algemene motor-functies
void motorVooruit(int in1, int in2, int en, int snelheid) {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(en, snelheid);
}

void motorAchteruit(int in1, int in2, int en, int snelheid) {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(en, snelheid);
}

void motorStop(int in1, int in2, int en) {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(en, 0);
}

void loop() {
  // Voorbeeld: elke motor apart testen

  Serial.println("Motor 1 vooruit");
  motorVooruit(M1_IN1, M1_IN2, M1_EN, 200);
  delay(1500);
  motorStop(M1_IN1, M1_IN2, M1_EN);

  Serial.println("Motor 2 achteruit");
  motorAchteruit(M2_IN1, M2_IN2, M2_EN, 180);
  delay(1500);
  motorStop(M2_IN1, M2_IN2, M2_EN);

  Serial.println("Motor 3 vooruit");
  motorVooruit(M3_IN1, M3_IN2, M3_EN, 220);
  delay(1500);
  motorStop(M3_IN1, M3_IN2, M3_EN);

  Serial.println("Motor 4 achteruit");
  motorAchteruit(M4_IN1, M4_IN2, M4_EN, 255);
  delay(1500);
  motorStop(M4_IN1, M4_IN2, M4_EN);

  delay(2000); // korte pauze
}
