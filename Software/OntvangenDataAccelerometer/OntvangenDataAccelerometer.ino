#include <ArduinoBLE.h>

BLEDevice peripheral;
BLECharacteristic accelChar;
bool connected = false;

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

      int16_t xi = word(data[1], data[0]);
      int16_t yi = word(data[3], data[2]);
      int16_t zi = word(data[5], data[4]);

      Serial.print("Ontvangen accel (mg): ");
      Serial.print(xi);
      Serial.print("\t");
      Serial.print(yi);
      Serial.print("\t");
      Serial.println(zi);
    }

    if (!peripheral.connected()) {
      Serial.println("Verbinding verbroken, opnieuw scannen...");
      connected = false;
      BLE.scan();
    }
  }
}
