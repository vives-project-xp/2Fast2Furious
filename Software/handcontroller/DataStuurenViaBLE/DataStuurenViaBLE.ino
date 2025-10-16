#include <ArduinoBLE.h>

BLEService imuService("12345678-1234-5678-1234-56789abcdef0");  
BLECharacteristic gyroCharacteristic("abcdef01-1234-5678-1234-56789abcdef0", BLERead | BLENotify, 2);

unsigned long previousMillis = 0;
const long interval = 500;
bool deviceConnected = false; // bijhouden of er een device verbonden is


void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("BLE start failed!");
    while (1);
  }

  BLE.setLocalName("NanoTests");
  BLE.setAdvertisedService(imuService);

  imuService.addCharacteristic(gyroCharacteristic);
  BLE.addService(imuService);

  BLE.advertise();
  Serial.println("Peripheral actief, wacht op verbinding...");
}

void loop() {
  BLE.poll();  // belangrijk

  // Check verbindingstatus
  if (BLE.connected() && !deviceConnected) {
    Serial.println("Er is een device verbonden!");
    deviceConnected = true;
  } 
  else if (!BLE.connected() && deviceConnected) {
    Serial.println("Device is verbroken.");
    deviceConnected = false;
  }

  // Timer voor versturen van data
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    int16_t waarde = random(-1000, 1000);
    byte data[2];
    data[0] = lowByte(waarde);
    data[1] = highByte(waarde);

    gyroCharacteristic.writeValue(data, 2);
    Serial.print("Verstuurd waarde: ");
    Serial.println(waarde);
  }
}
