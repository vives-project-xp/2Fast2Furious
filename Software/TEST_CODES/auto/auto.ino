#include <ArduinoBLE.h>
#include <Servo.h>

Servo myservo;

int pos = 0;

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

// BLE
BLEDevice peripheral;
BLECharacteristic commandChar;
bool connected = false;
String prevCommand = "";
int snelheid = 200;

void motorVooruit(int in1, int in2) {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
}

void motorAchteruit(int in1, int in2,) {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}

void motorStop(int in1, int in2) {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}

void vooruit() {
  motorVooruit(M1_IN1, M1_IN2, s);
  motorVooruit(M2_IN1, M2_IN2, s);
  motorVooruit(M3_IN1, M3_IN2, s);
  motorVooruit(M4_IN1, M4_IN2, s);
}
void achteruitint ()) {
  motorAchteruit(M1_IN1, M1_IN2, s);
  motorAchteruit(M2_IN1, M2_IN2, s);
  motorAchteruit(M3_IN1, M3_IN2, s);
  motorAchteruit(M4_IN1, M4_IN2, s);
}
void links() {
  motorAchteruit(M1_IN1, M1_IN2, s);
  motorVooruit(M2_IN1, M2_IN2, s);
  motorVooruit(M3_IN1, M3_IN2, s);
  motorAchteruit(M4_IN1, M4_IN2, s);
}
void rechts() {
  motorVooruit(M1_IN1, M1_IN2, s);
  motorAchteruit(M2_IN1, M2_IN2, s);
  motorAchteruit(M3_IN1, M3_IN2, s);
  motorVooruit(M4_IN1, M4_IN2, s);
}

void draaiCW() {  // met de klok mee
  motorVooruit(M1_IN1, M1_IN2, s);
  motorAchteruit(M2_IN1, M2_IN2, s);
  motorVooruit(M3_IN1, M3_IN2, s);
  motorAchteruit(M4_IN1, M4_IN2, s);
}

void draaiCCW() { // tegen de klok in
  motorAchteruit(M1_IN1, M1_IN2, s);
  motorVooruit(M2_IN1, M2_IN2, s);
  motorAchteruit(M3_IN1, M3_IN2, s);
  motorVooruit(M4_IN1, M4_IN2, s);
}
void stopAlles() {
  motorStop(M1_IN1, M1_IN2);
  motorStop(M2_IN1, M2_IN2);
  motorStop(M3_IN1, M3_IN2);
  motorStop(M4_IN1, M4_IN2);
}

void setup() {
  Serial.begin(9600);

  myservo.attach(9);

  // Motorpinnen
  pinMode(M1_IN1, OUTPUT);
  pinMode(M1_IN2, OUTPUT);
  pinMode(M2_IN1, OUTPUT);
  pinMode(M2_IN2, OUTPUT);
  pinMode(M3_IN1, OUTPUT);
  pinMode(M3_IN2, OUTPUT);
  pinMode(M4_IN1, OUTPUT);
  pinMode(M4_IN2, OUTPUT);

  // BLE setup
  if (!BLE.begin()) {
    Serial.println("BLE start failed!");
    while (1);
  }

  Serial.println("Central start, zoekt naar NanoController...");
  BLE.scan();
}

void loop() {
  if (!connected) {
    BLEDevice found = BLE.available();

    if (found && found.localName() == "NanoController") {
      Serial.println("Peripheral gevonden, verbinden...");
      BLE.stopScan();

      if (found.connect()) {
        Serial.println("Verbonden!");
        peripheral = found;
        connected = true;

        if (peripheral.discoverAttributes()) {
          commandChar = peripheral.characteristic("abcdef01-1234-5678-1234-56789abcdef0");
          if (commandChar) {
            commandChar.subscribe();
            Serial.println("Subscribed op command characteristic!");
          }
        }
      } else {
        Serial.println("Verbinding mislukt, opnieuw scannen...");
        BLE.scan();
      }
    }
  }

  if (connected && commandChar) {
    BLE.poll();

      if (commandChar.valueUpdated()) {
        uint8_t buffer[20];                
        int length = commandChar.readValue(buffer, sizeof(buffer));
        
        String cmd = "";   
        if (length > 0) {
          buffer[length] = '\0';           
          cmd = String((char*)buffer);
          Serial.print("Ontvangen commando: ");
          Serial.println(cmd);
        }
      
        if (cmd != prevCommand) {   
          prevCommand = cmd;
      
          if (cmd == "F") vooruit();
          else if (cmd == "B") achteruit();
          else if (cmd == "L") links();
          else if (cmd == "R") rechts();
          else if (cmd == "S") stopAlles();
          else if (cmd == "f") Serial.println("Loop Omhoog");
          else if (cmd == "b") Serial.println("Loop Omlaag");
          else if (cmd == "l") draaiCCW();
          else if (cmd == "r") draaiCW();
          else stopAlles();
        }
      }


    if (!peripheral.connected()) {
      Serial.println("Verbinding verbroken, opnieuw scannen...");
      connected = false;
      BLE.scan();
    }
  }
}
