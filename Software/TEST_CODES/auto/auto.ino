#include <ArduinoBLE.h>

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

// BLE
BLEDevice peripheral;
BLECharacteristic commandChar;
bool connected = false;
String prevCommand = "";
int snelheid = 200;

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

void vooruit(int s) {
  motorVooruit(M1_IN1, M1_IN2, M1_EN, s);
  motorVooruit(M2_IN1, M2_IN2, M2_EN, s);
  motorVooruit(M3_IN1, M3_IN2, M3_EN, s);
  motorVooruit(M4_IN1, M4_IN2, M4_EN, s);
}
void achteruit(int s) {
  motorAchteruit(M1_IN1, M1_IN2, M1_EN, s);
  motorAchteruit(M2_IN1, M2_IN2, M2_EN, s);
  motorAchteruit(M3_IN1, M3_IN2, M3_EN, s);
  motorAchteruit(M4_IN1, M4_IN2, M4_EN, s);
}
void links(int s) {
  motorAchteruit(M1_IN1, M1_IN2, M1_EN, s);
  motorVooruit(M2_IN1, M2_IN2, M2_EN, s);
  motorVooruit(M3_IN1, M3_IN2, M3_EN, s);
  motorAchteruit(M4_IN1, M4_IN2, M4_EN, s);
}
void rechts(int s) {
  motorVooruit(M1_IN1, M1_IN2, M1_EN, s);
  motorAchteruit(M2_IN1, M2_IN2, M2_EN, s);
  motorAchteruit(M3_IN1, M3_IN2, M3_EN, s);
  motorVooruit(M4_IN1, M4_IN2, M4_EN, s);
}
void stopAlles() {
  motorStop(M1_IN1, M1_IN2, M1_EN);
  motorStop(M2_IN1, M2_IN2, M2_EN);
  motorStop(M3_IN1, M3_IN2, M3_EN);
  motorStop(M4_IN1, M4_IN2, M4_EN);
}

void setup() {
  Serial.begin(9600);

  // Motorpinnen
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
      
          if (cmd == "F") vooruit(snelheid);
          else if (cmd == "B") achteruit(snelheid);
          else if (cmd == "L") links(snelheid);
          else if (cmd == "R") rechts(snelheid);
          else if (cmd == "S") stopAlles();
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
