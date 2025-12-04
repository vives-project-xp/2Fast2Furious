/*
  Simplified Magic Glove example. Detects circular motion.
  No TensorFlow model -> uses light geometric analysis of gyro orientation to detect circles.
*/

#include <Arduino.h>
#include <Arduino_LSM9DS1.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Config
constexpr int kMaxStrokePoints = 800;   // max points to store per stroke
constexpr float kMovingGyroThreshold = 15.0f;   // degrees/sec magnitude to consider "moving"
constexpr int kStopSamplesRequired = 10;  // number of consecutive low-gyro samples to end stroke
constexpr int kMinStrokeLength = 20; // minimum number of samples to consider

// State
enum StrokeState { WAITING = 0, DRAWING = 1 };
StrokeState stroke_state = WAITING;

float gyroscope_sample_rate = 0.0f; // samples/sec
float orientation_x = 0.0f; // degrees
float orientation_y = 0.0f; // degrees

struct Point { float x; float y; };
Point stroke_points[kMaxStrokePoints];
int stroke_length = 0;

int low_gyro_count = 0; // consecutive samples under threshold

void setup() {
  Serial.begin(115200); // open Serial Monitor at 115200 baud & follow printed messages
  while (!Serial) { delay(10); }
  Serial.println("magic_glove starting...");

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU! Make sure you have an LSM9DS1-based board.");
    while (1) { delay(1000); }
  }

  // Use continuous sampling so we can drain FIFO
  IMU.setContinuousMode();
  gyroscope_sample_rate = IMU.gyroscopeSampleRate();
  if (gyroscope_sample_rate <= 0.0f) {
    // fallback - assume 119 Hz typical for LSM9DS1 FIFO
    gyroscope_sample_rate = 119.0f;
  }

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.print("Gyro sample rate: "); Serial.println(gyroscope_sample_rate);
  Serial.println("Ready. Wave the wand and try drawing a circle.");
}

// Simple helper: magnitude of a 3-vector
static float Vec3Mag(float x, float y, float z) {
  return sqrtf(x*x + y*y + z*z);
}

// When done drawing, analyze stroke to see if it is a circle
bool AnalyzeStrokeForCircle() {
  if (stroke_length < kMinStrokeLength) return false;

  // Compute centroid
  double cx = 0.0, cy = 0.0;
  for (int i = 0; i < stroke_length; ++i) {
    cx += stroke_points[i].x;
    cy += stroke_points[i].y;
  }
  cx /= stroke_length;
  cy /= stroke_length;

  // Compute radii and angles in time order
  double radii_sum = 0.0;
  double radii_sq_sum = 0.0;
  double angles[kMaxStrokePoints];
  for (int i = 0; i < stroke_length; ++i) {
    double dx = stroke_points[i].x - cx;
    double dy = stroke_points[i].y - cy;
    double r = sqrt(dx*dx + dy*dy);
    radii_sum += r;
    radii_sq_sum += r*r;
    angles[i] = atan2(dy, dx);
  }
  double r_mean = radii_sum / stroke_length;
  double r_var = (radii_sq_sum / stroke_length) - (r_mean * r_mean);
  if (r_var < 0.0) r_var = 0.0;
  double r_std = sqrt(r_var);

  // Radius consistency: std/mean should be reasonably small
  double radius_ratio = (r_mean > 0.0) ? (r_std / r_mean) : 1e9;

  // Compute total angular traversal (unwrap angles in time order)
  double total_angular = 0.0;
  double prev = angles[0];
  for (int i = 1; i < stroke_length; ++i) {
    double a = angles[i];
    // unwrap
    while (a - prev > M_PI) a -= (2.0 * M_PI);
    while (a - prev < -M_PI) a += (2.0 * M_PI);
    total_angular += fabs(a - prev);
    prev = a;
  }

  // Heuristics for circle:
  // - Total angular traversal should be at least ~300 degrees (5.2 rad)
  // - Radius should be not tiny, and reasonably consistent
  const double kMinAngular = 5.2; // ~300 degrees in radians
  const double kMaxRadiusRatio = 0.45; // allow up to 45% variation
  const double kMinRadius = 2.0; // in degrees (orientation space), avoid tiny jitter

  Serial.print("Stroke length = "); Serial.println(stroke_length);
  Serial.print("Mean radius = "); Serial.print(r_mean, 3);
  Serial.print("  std = "); Serial.print(r_std, 3);
  Serial.print("  ratio = "); Serial.println(radius_ratio, 3);
  Serial.print("Total angular (rad) = "); Serial.println(total_angular, 3);

  bool angular_ok = (total_angular >= kMinAngular);
  bool radius_ok = (r_mean >= kMinRadius) && (radius_ratio <= kMaxRadiusRatio);

  return angular_ok && radius_ok;
}

void ResetStroke() {
  stroke_length = 0;
  low_gyro_count = 0;
}

void loop() {
  // drain FIFO; for each set of gyro samples, integrate into orientation
  while (IMU.gyroscopeAvailable()) {
    float gx, gy, gz;
    if (!IMU.readGyroscope(gx, gy, gz)) {
      Serial.println("Failed to read gyroscope");
      break;
    }

    // Simple drift compensation: when we're in WAITING and stationary, we don't integrate
    // For simplicity we won't actively estimate drift here; this is a pragmatic approach.

    // Integrate orientation: gyro units are degrees/sec; divide by sample rate to get degrees per sample
    const float dt = 1.0f / gyroscope_sample_rate;
    orientation_x += gx * dt; // pitch-like
    orientation_y += gy * dt; // yaw-like

    // Compute gyro magnitude to detect motion
    float gyro_mag = Vec3Mag(gx, gy, gz);

    if (stroke_state == WAITING) {
      if (gyro_mag > kMovingGyroThreshold) {
        // Start drawing
        stroke_state = DRAWING;
        ResetStroke();
        // capture first point
        if (stroke_length < kMaxStrokePoints) {
          stroke_points[stroke_length++] = { orientation_x, orientation_y };
        }
        digitalWrite(LED_BUILTIN, HIGH);
      }
    } else if (stroke_state == DRAWING) {
      // while drawing, store samples
      if (stroke_length < kMaxStrokePoints) {
        stroke_points[stroke_length++] = { orientation_x, orientation_y };
      }

      // detect potential stop: consecutive low gyro samples
      if (gyro_mag <= kMovingGyroThreshold) {
        low_gyro_count++;
      } else {
        low_gyro_count = 0;
      }

      if (low_gyro_count >= kStopSamplesRequired) {
        // end stroke
        digitalWrite(LED_BUILTIN, LOW);
        stroke_state = WAITING;

        // Analyze the stroke for a circle
        bool is_circle = AnalyzeStrokeForCircle();
        if (is_circle) {
          Serial.println("=== CIRCLE DETECTED! ===");
          // blink LED to indicate detection
          for (int i = 0; i < 3; ++i) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(150);
            digitalWrite(LED_BUILTIN, LOW);
            delay(150);
          }
        } else {
          Serial.println("Stroke not a circle");
        }

        ResetStroke();
        // small delay so we don't re-trigger immediately
        delay(50);
      }
    }
  }

  // keep loop responsive
  delay(1);
}
