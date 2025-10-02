#include <ArduinoBLE.h>

BLEDevice peripheral;
BLECharacteristic gyroChar;
bool connected = false;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("BLE start failed!");
    while (1);
  }

  Serial.println("Central start, zoekt naar Peripheral...");
  BLE.scan();  // start scannen
}

void loop() {
  if (!connected) {
    BLEDevice found = BLE.available();

    if (found && found.localName() == "NanoTests") {
      Serial.println("Peripheral gevonden, verbinden...");

      BLE.stopScan(); // <<< heel belangrijk!

      if (found.connect()) {
        Serial.println("Verbonden!");
        peripheral = found;
        connected = true;

        if (peripheral.discoverAttributes()) {
          gyroChar = peripheral.characteristic("abcdef01-1234-5678-1234-56789abcdef0");

          if (gyroChar) {
            gyroChar.subscribe();
            Serial.println("Subscribed op characteristic!");
          }
        }
      } else {
        Serial.println("Verbinding mislukt, opnieuw scannen...");
        BLE.scan();
      }
    }
  }

  if (connected && gyroChar) {
    BLE.poll(); // events en notify verwerken

    if (gyroChar.valueUpdated()) {
      byte data[2];
      gyroChar.readValue(data, 2);
      int16_t waarde = word(data[1], data[0]);
      Serial.print("Ontvangen waarde: ");
      Serial.println(waarde);
    }

    if (!peripheral.connected()) {
      Serial.println("Verbinding verbroken, opnieuw scannen...");
      connected = false;
      BLE.scan();
    }
  }
}
