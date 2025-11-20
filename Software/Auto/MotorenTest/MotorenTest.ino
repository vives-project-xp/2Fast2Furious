// Motor 1
#define M1_IN1 2
#define M1_IN2 3

// Motor 2
#define M2_IN1 5
#define M2_IN2 6

// Motor 3
#define M3_IN1 10
#define M3_IN2 11

// Motor 4
#define M4_IN1 13
#define M4_IN2 8

void setup() {
  pinMode(M1_IN1, OUTPUT);
  pinMode(M1_IN2, OUTPUT);

  pinMode(M2_IN1, OUTPUT);
  pinMode(M2_IN2, OUTPUT);

  pinMode(M3_IN1, OUTPUT);
  pinMode(M3_IN2, OUTPUT);

  pinMode(M4_IN1, OUTPUT);
  pinMode(M4_IN2, OUTPUT);

  Serial.begin(9600);
}


void motorVooruit(int in1, int in2) {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
}

void motorAchteruit(int in1, int in2) {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}

void motorStop(int in1, int in2) {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}


void vooruit() {
  motorVooruit(M1_IN1, M1_IN2);
  motorVooruit(M2_IN1, M2_IN2);
  motorVooruit(M3_IN1, M3_IN2);
  motorVooruit(M4_IN1, M4_IN2);
}

void achteruit() {
  motorAchteruit(M1_IN1, M1_IN2);
  motorAchteruit(M2_IN1, M2_IN2);
  motorAchteruit(M3_IN1, M3_IN2);
  motorAchteruit(M4_IN1, M4_IN2);
}

void links() {
  motorAchteruit(M1_IN1, M1_IN2);
  motorVooruit(M2_IN1, M2_IN2);
  motorVooruit(M3_IN1, M3_IN2);
  motorAchteruit(M4_IN1, M4_IN2);
}

void rechts() {
  motorVooruit(M1_IN1, M1_IN2);
  motorAchteruit(M2_IN1, M2_IN2);
  motorAchteruit(M3_IN1, M3_IN2);
  motorVooruit(M4_IN1, M4_IN2);
}

void draaiCW() {  // met de klok mee
  motorVooruit(M1_IN1, M1_IN2);
  motorAchteruit(M2_IN1, M2_IN2);
  motorVooruit(M3_IN1, M3_IN2);
  motorAchteruit(M4_IN1, M4_IN2);
}

void draaiCCW() { // tegen de klok in
  motorAchteruit(M1_IN1, M1_IN2);
  motorVooruit(M2_IN1, M2_IN2);
  motorAchteruit(M3_IN1, M3_IN2);
  motorVooruit(M4_IN1, M4_IN2);
}

void stopAlles() {
  motorStop(M1_IN1, M1_IN2);
  motorStop(M2_IN1, M2_IN2);
  motorStop(M3_IN1, M3_IN2);
  motorStop(M4_IN1, M4_IN2);
}


void loop() {
  Serial.println("Vooruit");
  vooruit();
  delay(2000);
  stopAlles();
  delay(1000);

  Serial.println("Achteruit");
  achteruit();
  delay(2000);
  stopAlles();
  delay(1000);

  Serial.println("Links");
  links();
  delay(2000);
  stopAlles();
  delay(1000);

  Serial.println("Rechts");
  rechts();
  delay(2000);
  stopAlles();
  delay(1000);

  Serial.println("Draaien CW");
  draaiCW();
  delay(2000);
  stopAlles();
  delay(1000);

  Serial.println("Draaien CCW");
  draaiCCW();
  delay(2000);
  stopAlles();
  delay(2000);
}
