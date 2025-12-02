// Simpel voorbeeld: aan/uit en dimmen met PWM
const int laserPin = 7; // kies een PWM-capabele pin

void setup() {
  pinMode(laserPin, OUTPUT);
  digitalWrite(laserPin, LOW); // veilig starten: uit
}

void loop() {
  // Zet laser aan constant
  //digitalWrite(laserPin, HIGH);

  digitalWrite(laserPin, HIGH);
  delay(1000);
  digitalWrite(laserPin, LOW);
  delay(1000);
}
