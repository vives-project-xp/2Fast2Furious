#include <ArduinoBLE.h>

int16_t xi = 0;
int16_t yi = 0;
int16_t zi = 0;

BLEDevice peripheral;
BLECharacteristic accelChar;
bool connected = false;

// Houd de vorige beweging bij
String prevState = "";

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("BLE start failed!");
    while (1);
  }

  Serial.println("Central start, zoekt naar Peripheral...");
  BLE.scan();
}

void loop() {
  if (!connected) {
    BLEDevice found = BLE.available();

    if (found && found.localName() == "NanoTests") {
      Serial.println("Peripheral gevonden, verbinden...");

      BLE.stopScan();

      if (found.connect()) {
        Serial.println("Verbonden!");
        peripheral = found;
        connected = true;

        if (peripheral.discoverAttributes()) {
          accelChar = peripheral.characteristic("abcdef01-1234-5678-1234-56789abcdef0");
          if (accelChar) {
            accelChar.subscribe();
            Serial.println("Subscribed op accelerometer characteristic!");
          }
        }
      } else {
        Serial.println("Verbinding mislukt, opnieuw scannen...");
        BLE.scan();
      }
    }
  }

  if (connected && accelChar) {
    BLE.poll();

    if (accelChar.valueUpdated()) {
      byte data[6];
      accelChar.readValue(data, 6);

      xi = word(data[1], data[0]);
      yi = word(data[3], data[2]);
      zi = word(data[5], data[4]);
    }

    // Drempels
    int singleThreshold = 700; // Voor enkelvoudige richtingen
    int diagonalThreshold = 300; // Voor diagonale richtingen

    String currentState = "";

    // Eerst diagonalen checken (lagere drempel)
    if (xi > diagonalThreshold && yi > diagonalThreshold) {
      currentState = "Rechts-Vooruit";
    } else if (xi > diagonalThreshold && yi < -diagonalThreshold) {
      currentState = "Links-Vooruit";
    } else if (xi < -diagonalThreshold && yi > diagonalThreshold) {
      currentState = "Rechts-Achteruit";
    } else if (xi < -diagonalThreshold && yi < -diagonalThreshold) {
      currentState = "Links-Achteruit";
    } 
    // Enkelvoudige richtingen checken (hogere drempel)
    else if (xi > singleThreshold) {
      currentState = "Vooruit";
    } else if (xi < -singleThreshold) {
      currentState = "Achteruit";
    } else if (yi > singleThreshold) {
      currentState = "Rechts";
    } else if (yi < -singleThreshold) {
      currentState = "Links";
    } else {
      currentState = "Still";
    }

    // Print alleen als status verandert
    if (currentState != prevState) {
      Serial.println(currentState);
      prevState = currentState;
    }

    if (!peripheral.connected()) {
      Serial.println("Verbinding verbroken, opnieuw scannen...");
      connected = false;
      BLE.scan();
    }
  }
}
