#include <ArduinoBLE.h>
#include "Arduino_BMI270_BMM150.h"

// BLE Service en Characteristic
BLEService imuService("12345678-1234-5678-1234-56789abcdef0");  
// 6 bytes (3x int16_t voor X, Y, Z)
BLECharacteristic accelCharacteristic("abcdef01-1234-5678-1234-56789abcdef0", BLERead | BLENotify, 6);

unsigned long previousMillis = 0;
const long interval = 100;
bool deviceConnected = false;

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

  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");

  // BLE setup
  BLE.setLocalName("NanoTests");
  BLE.setAdvertisedService(imuService);

  imuService.addCharacteristic(accelCharacteristic);
  BLE.addService(imuService);

  BLE.advertise();
  Serial.println("Peripheral actief, wacht op verbinding...");
}

void loop() {
  BLE.poll();

  // Verbinding status checken
  if (BLE.connected() && !deviceConnected) {
    Serial.println("Er is een device verbonden!");
    deviceConnected = true;
  } 
  else if (!BLE.connected() && deviceConnected) {
    Serial.println("Device is verbroken.");
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
      int16_t zi = (int16_t)(z * 1000);

      // In 6 bytes stoppen (little endian)
      byte data[6];
      data[0] = lowByte(xi);
      data[1] = highByte(xi);
      data[2] = lowByte(yi);
      data[3] = highByte(yi);
      data[4] = lowByte(zi);
      data[5] = highByte(zi);

      accelCharacteristic.writeValue(data, 6);

      Serial.print("Verstuurd accel: ");
      Serial.print(xi);
      Serial.print("\t");
      Serial.print(yi);
      Serial.print("\t");
      Serial.println(zi);
    }
  }
}
