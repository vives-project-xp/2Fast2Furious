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


void vooruit(int snelheid) {
  motorVooruit(M1_IN1, M1_IN2, M1_EN, snelheid);
  motorVooruit(M2_IN1, M2_IN2, M2_EN, snelheid);
  motorVooruit(M3_IN1, M3_IN2, M3_EN, snelheid);
  motorVooruit(M4_IN1, M4_IN2, M4_EN, snelheid);
}

void achteruit(int snelheid) {
  motorAchteruit(M1_IN1, M1_IN2, M1_EN, snelheid);
  motorAchteruit(M2_IN1, M2_IN2, M2_EN, snelheid);
  motorAchteruit(M3_IN1, M3_IN2, M3_EN, snelheid);
  motorAchteruit(M4_IN1, M4_IN2, M4_EN, snelheid);
}

void links(int snelheid) {
  motorAchteruit(M1_IN1, M1_IN2, M1_EN, snelheid);
  motorVooruit(M2_IN1, M2_IN2, M2_EN, snelheid);
  motorVooruit(M3_IN1, M3_IN2, M3_EN, snelheid);
  motorAchteruit(M4_IN1, M4_IN2, M4_EN, snelheid);
}

void rechts(int snelheid) {
  motorVooruit(M1_IN1, M1_IN2, M1_EN, snelheid);
  motorAchteruit(M2_IN1, M2_IN2, M2_EN, snelheid);
  motorAchteruit(M3_IN1, M3_IN2, M3_EN, snelheid);
  motorVooruit(M4_IN1, M4_IN2, M4_EN, snelheid);
}

void draaiCW(int snelheid) {  // met de klok mee
  motorVooruit(M1_IN1, M1_IN2, M1_EN, snelheid);
  motorAchteruit(M2_IN1, M2_IN2, M2_EN, snelheid);
  motorVooruit(M3_IN1, M3_IN2, M3_EN, snelheid);
  motorAchteruit(M4_IN1, M4_IN2, M4_EN, snelheid);
}

void draaiCCW(int snelheid) { // tegen de klok in
  motorAchteruit(M1_IN1, M1_IN2, M1_EN, snelheid);
  motorVooruit(M2_IN1, M2_IN2, M2_EN, snelheid);
  motorAchteruit(M3_IN1, M3_IN2, M3_EN, snelheid);
  motorVooruit(M4_IN1, M4_IN2, M4_EN, snelheid);
}

void stopAlles() {
  motorStop(M1_IN1, M1_IN2, M1_EN);
  motorStop(M2_IN1, M2_IN2, M2_EN);
  motorStop(M3_IN1, M3_IN2, M3_EN);
  motorStop(M4_IN1, M4_IN2, M4_EN);
}


void loop() {
  Serial.println("Vooruit");
  vooruit(200);
  delay(2000);
  stopAlles();
  delay(1000);

  Serial.println("Achteruit");
  achteruit(200);
  delay(2000);
  stopAlles();
  delay(1000);

  Serial.println("Links");
  links(200);
  delay(2000);
  stopAlles();
  delay(1000);

  Serial.println("Rechts");
  rechts(200);
  delay(2000);
  stopAlles();
  delay(1000);

  Serial.println("Draaien CW");
  draaiCW(200);
  delay(2000);
  stopAlles();
  delay(1000);

  Serial.println("Draaien CCW");
  draaiCCW(200);
  delay(2000);
  stopAlles();
  delay(2000);
}
