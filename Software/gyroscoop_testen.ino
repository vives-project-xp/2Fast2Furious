#include <Wire.h>
#define MPU_ADDR 0x68

// calibration & filter settings
const int CALIB_SAMPLES = 500;
const float GYRO_SCALE = 131.0;
const float ACCEL_SCALE = 16384.0;
const float MY_RAD_TO_DEG = 57.29577951308232f;
const float ALPHA = 0.98;
const float GYRO_DEADBAND = 0.5


float gyroBiasX = 0, gyroBiasY = 0, gyroBiasZ = 0;
float angleX = 0, angleY = 0;
unsigned long lastMicros = 0;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  delay(100);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);
  delay(50);

  Serial.println("MPU-9250 complementary filter (pitch/roll). Calibrating...");

  calibrateGyro();

  lastMicros = micros();

  Serial.println("Ready. Move sensor to see angles. Pitch & Roll shown in degrees.");
  Serial.println("Format: Roll, Pitch");
}

void loop() {
  // read accel (6) + temp (2) + gyro (6) in one burst
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // ACCEL_XOUT_H
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);

  int16_t ax = (Wire.read() << 8) | Wire.read(); // ACCEL_X
  int16_t ay = (Wire.read() << 8) | Wire.read(); // ACCEL_Y
  int16_t az = (Wire.read() << 8) | Wire.read(); // ACCEL_Z
  int16_t temp = (Wire.read() << 8) | Wire.read(); // TEMP (not used)
  int16_t gx = (Wire.read() << 8) | Wire.read(); // GYRO_X
  int16_t gy = (Wire.read() << 8) | Wire.read(); // GYRO_Y
  int16_t gz = (Wire.read() << 8) | Wire.read(); // GYRO_Z

  // convert
  float ax_g = (float)ax / ACCEL_SCALE;
  float ay_g = (float)ay / ACCEL_SCALE;
  float az_g = (float)az / ACCEL_SCALE;

  float gx_dps = ((float)gx / GYRO_SCALE) - gyroBiasX;
  float gy_dps = ((float)gy / GYRO_SCALE) - gyroBiasY;
  float gz_dps = ((float)gz / GYRO_SCALE) - gyroBiasZ;

  // time delta
  unsigned long now = micros();
  float dt = (now - lastMicros) / 1000000.0f;
  if (dt <= 0) dt = 0.001f;
  lastMicros = now;

  // --- accelerometer angles (degrees)
  float accRoll  = atan2(ay_g, az_g) * MY_RAD_TO_DEG;
  float accPitch = atan2(-ax_g, sqrt(ay_g*ay_g + az_g*az_g)) * MY_RAD_TO_DEG;

  // --- apply deadband to gyro (avoid integrating tiny noise)
  if (fabs(gx_dps) < GYRO_DEADBAND) gx_dps = 0;
  if (fabs(gy_dps) < GYRO_DEADBAND) gy_dps = 0;
  if (fabs(gz_dps) < GYRO_DEADBAND) gz_dps = 0;

  // --- integrate gyro to angles
  float gyroAngleX = angleX + gx_dps * dt;
  float gyroAngleY = angleY + gy_dps * dt;

  // --- complementary filter: combine gyro (short term) + accel (long term)
  angleX = ALPHA * gyroAngleX + (1.0f - ALPHA) * accRoll;
  angleY = ALPHA * gyroAngleY + (1.0f - ALPHA) * accPitch;

  // print angles
  Serial.print("GyX: ");
  Serial.print(angleX, 2);
  Serial.print("  | GyY: ");
  Serial.print(angleY, 2);
  Serial.println();

  delay(10);
}

// simple gyro bias calibration: take multiple samples while sensor is held still
void calibrateGyro() {
  long sumGx = 0, sumGy = 0, sumGz = 0;
  for (int i = 0; i < CALIB_SAMPLES; ++i) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, 14, true);

    Wire.read(); Wire.read(); // ax
    Wire.read(); Wire.read(); // ay
    Wire.read(); Wire.read(); // az
    Wire.read(); Wire.read(); // temp
    int16_t gx = (Wire.read() << 8) | Wire.read();
    int16_t gy = (Wire.read() << 8) | Wire.read();
    int16_t gz = (Wire.read() << 8) | Wire.read();

    sumGx += gx;
    sumGy += gy;
    sumGz += gz;
    delay(5);
  }
  gyroBiasX = ((float)sumGx / CALIB_SAMPLES) / GYRO_SCALE;
  gyroBiasY = ((float)sumGy / CALIB_SAMPLES) / GYRO_SCALE;
  gyroBiasZ = ((float)sumGz / CALIB_SAMPLES) / GYRO_SCALE;

  Serial.print("Gyro biases (°/s): ");
  Serial.print(gyroBiasX, 4); Serial.print(", ");
  Serial.print(gyroBiasY, 4); Serial.print(", ");
  Serial.println(gyroBiasZ, 4);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);
  int16_t ax = (Wire.read() << 8) | Wire.read();
  int16_t ay = (Wire.read() << 8) | Wire.read();
  int16_t az = (Wire.read() << 8) | Wire.read();

  float ax_g = (float)ax / ACCEL_SCALE;
  float ay_g = (float)ay / ACCEL_SCALE;
  float az_g = (float)az / ACCEL_SCALE;

  angleX = atan2(ay_g, az_g) * MY_RAD_TO_DEG;
  angleY = atan2(-ax_g, sqrt(ay_g*ay_g + az_g*az_g)) * MY_RAD_TO_DEG;
}
