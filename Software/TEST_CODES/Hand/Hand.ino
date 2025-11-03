#include <ArduinoBLE.h>
#include "Arduino_BMI270_BMM150.h"

// BLE Service en Characteristic
BLEService controlService("12345678-1234-5678-1234-56789abcdef0");
BLEStringCharacteristic commandCharacteristic("abcdef01-1234-5678-1234-56789abcdef0", BLERead | BLENotify, 20);

unsigned long previousMillis = 0;
const long interval = 100;  // update-interval (ms)
bool deviceConnected = false;
String prevCommand = "";

// --- Richtingsdetectie functie ---
String getCommand(float x, float y) {
  int singleThreshold = 0.7;     // m/s² drempels
  int diagonalThreshold = 0.3;

  if (x > diagonalThreshold && y > diagonalThreshold) return "FR"; // Forward-Right
  if (x > diagonalThreshold && y < -diagonalThreshold) return "FL";
  if (x < -diagonalThreshold && y > diagonalThreshold) return "BR";
  if (x < -diagonalThreshold && y < -diagonalThreshold) return "BL";
  if (x > singleThreshold) return "F";   // Vooruit
  if (x < -singleThreshold) return "B";  // Achteruit
  if (y > singleThreshold) return "R";   // Rechts
  if (y < -singleThreshold) return "L";  // Links
  return "S";                            // Stilstand
}

void setup() {
  Serial.begin(9600);

  // Start BLE
  if (!BLE.begin()) {
    Serial.println("BLE start failed!");
    while (1);
  }

  // Start IMU
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  BLE.setLocalName("NanoController");
  BLE.setAdvertisedService(controlService);
  controlService.addCharacteristic(commandCharacteristic);
  BLE.addService(controlService);
  BLE.advertise();

  Serial.println("Zender actief — wacht op verbinding...");
}

void loop() {
  BLE.poll();

  if (BLE.connected() && !deviceConnected) {
    Serial.println("Verbonden!");
    deviceConnected = true;
  } else if (!BLE.connected() && deviceConnected) {
    Serial.println("Verbinding verbroken.");
    deviceConnected = false;
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    float x, y, z;
    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(x, y, z);

      String currentCommand = getCommand(x, y);

      if (currentCommand != prevCommand) {
        commandCharacteristic.writeValue(currentCommand);
        Serial.print("Verstuurd commando: ");
        Serial.println(currentCommand);
        prevCommand = currentCommand;
      }
    }
  }
}
