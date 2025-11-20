#include <ArduinoBLE.h>
#include "Arduino_BMI270_BMM150.h"

// BLE Service en Characteristic
BLEService controlService("12345678-1234-5678-1234-56789abcdef0");
BLEStringCharacteristic commandCharacteristic("abcdef01-1234-5678-1234-56789abcdef0", BLERead | BLENotify, 20);

unsigned long previousMillis = 0;
const long interval = 100;  
bool deviceConnected = false;
bool defaultMode = true;
String prevCommand = "";


String getCommand(float xi, float yi) {
  // Drempels
  
  //Serial.print("X : ");
  //Serial.println(xi);
  //Serial.print("Y : ");
  //Serial.println(yi);
  
  int singleThreshold = 700;     // Voor enkelvoudige richtingen
  int diagonalThreshold = 300;   // Voor diagonale richtingen

  // Eerst diagonalen checken (lagere drempel)
  if (xi > diagonalThreshold && yi > diagonalThreshold) {
    return "FL";
  } else if (xi > diagonalThreshold && yi < -diagonalThreshold) {
    return "FR";
  } else if (xi < -diagonalThreshold && yi > diagonalThreshold) {
    return "BL";
  } else if (xi < -diagonalThreshold && yi < -diagonalThreshold) {
    return "BR";
  } 
  // Enkelvoudige richtingen checken (hogere drempel)
  else if (xi > singleThreshold) {
    return "F";
  } else if (xi < -singleThreshold) {
    return "B";
  } else if (yi > singleThreshold) {
    return "L";
  } else if (yi < -singleThreshold) {
    return "R";
  } 
  else {
    return "S";
  }                          // Stilstand
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

      // Zet om naar milli-g en cast naar int16_t
      int16_t xi = (int16_t)(x * 1000);
      int16_t yi = (int16_t)(y * 1000);
      //int16_t zi = (int16_t)(z * 1000);

      //if(defaultMode) {
        String currentCommand = getCommand(xi, yi);

      //}
      
      if (currentCommand != prevCommand) {
        commandCharacteristic.writeValue(currentCommand);
        Serial.print("Verstuurd commando: ");
        Serial.println(currentCommand);
        prevCommand = currentCommand;
      }
    }
  }
}
