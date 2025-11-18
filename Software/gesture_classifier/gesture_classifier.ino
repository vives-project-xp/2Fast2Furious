// TensorFlow Lite Micro gesture classifier
// Uses Arduino Nano 33 BLE Rev.2 with BMI270/BMM150 IMU

#include <Arduino_BMI270_BMM150.h>
#include <TensorFlowLite.h>
#include <tensorflow/lite/micro/all_ops_resolver.h>
#include <tensorflow/lite/micro/micro_error_reporter.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/schema/schema_generated.h>
#include <tensorflow/lite/version.h>

#include "gesture_model.h"

// Settings
const int NUM_SAMPLES = 200;          // Number of samples per gesture
const int NUM_AXES = 6;               // 3 accel + 3 gyro
const int NUM_FEATURES = 24;          // 4 stats per axis (mean, std, min, max)
const int NUM_CLASSES = 3;

// Labels matching training order
const char* GESTURES[] = {"Punch", "circle", "idle"};

// TFLite globals
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
tflite::ErrorReporter* error_reporter = nullptr;

// Memory arena for TFLite
constexpr int kTensorArenaSize = 60 * 1024;
alignas(16) uint8_t tensor_arena[kTensorArenaSize];

// Buffers for sensor data
float sample_buffer[NUM_SAMPLES][NUM_AXES];
int samples_read = 0;

// Preprocessing params (from model_params.npz)
float feature_mean[NUM_FEATURES] = {
  -0.0086, 1.5866, -3.8708, 3.5721, 0.0230, 1.8307,
  -5.0121, 5.2136, 9.5987, 1.3837, 5.2731, 13.6194,
  -0.8927, 23.3695, -61.3211, 63.1349, 0.3478, 26.2515,
  -66.5243, 70.0886, 0.3911, 26.0975, -64.4345, 67.9083
};
float feature_std[NUM_FEATURES] = {
  0.5079, 1.3467, 3.2853, 3.2297, 0.5279, 1.2310,
  3.3168, 3.2202, 0.3231, 0.8846, 3.2046, 2.4673,
  5.0700, 24.5948, 52.3558, 52.8383, 3.6086, 19.1646,
  50.0784, 48.7146, 1.7509, 20.9778, 51.2285, 47.8745
};

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Initialize IMU
  if (!IMU.begin()) {
    Serial.println("ERROR: IMU initialization failed!");
    while (1);
  }

  Serial.println("IMU initialized");
  Serial.print("Acceleration sample rate: ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");

  // Set up error reporter
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Load TFLite model
  model = tflite::GetModel(gesture_model);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("ERROR: Model schema mismatch!");
    while (1);
  }

  // Initialize TFLite interpreter
  static tflite::AllOpsResolver resolver;
  static tflite::MicroInterpreter static_interpreter(
    model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  // Allocate tensors
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    Serial.println("ERROR: AllocateTensors() failed");
    while (1);
  }

  // Get input/output tensors
  input = interpreter->input(0);
  output = interpreter->output(0);

  Serial.println("TFLite model loaded successfully");
  Serial.println("Gesture classifier ready!");
  Serial.println("Perform a gesture to classify...\n");
}

void extractFeatures(float features[NUM_FEATURES]) {
  // Extract statistical features per axis
  for (int axis = 0; axis < NUM_AXES; axis++) {
    float sum = 0, sum_sq = 0;
    float min_val = sample_buffer[0][axis];
    float max_val = sample_buffer[0][axis];

    // Calculate sum, min, max
    for (int i = 0; i < NUM_SAMPLES; i++) {
      float val = sample_buffer[i][axis];
      sum += val;
      sum_sq += val * val;
      if (val < min_val) min_val = val;
      if (val > max_val) max_val = val;
    }

    // Calculate mean and std
    float mean = sum / NUM_SAMPLES;
    float variance = (sum_sq / NUM_SAMPLES) - (mean * mean);
    float std_dev = sqrt(variance > 0 ? variance : 0);

    // Store features: mean, std, min, max for each axis
    int base_idx = axis * 4;
    features[base_idx + 0] = mean;
    features[base_idx + 1] = std_dev;
    features[base_idx + 2] = min_val;
    features[base_idx + 3] = max_val;
  }
}

void normalizeFeatures(float features[NUM_FEATURES]) {
  for (int i = 0; i < NUM_FEATURES; i++) {
    features[i] = (features[i] - feature_mean[i]) / feature_std[i];
  }
}

void loop() {
  float aX, aY, aZ, gX, gY, gZ;

  // Collect samples
  while (samples_read < NUM_SAMPLES) {
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
      IMU.readAcceleration(aX, aY, aZ);
      IMU.readGyroscope(gX, gY, gZ);

      // Convert acceleration from g to m/s² to match training data (1g = 9.80665 m/s²)
      sample_buffer[samples_read][0] = aX * 9.80665;
      sample_buffer[samples_read][1] = aY * 9.80665;
      sample_buffer[samples_read][2] = aZ * 9.80665;
      sample_buffer[samples_read][3] = gX;
      sample_buffer[samples_read][4] = gY;
      sample_buffer[samples_read][5] = gZ;

      samples_read++;
    }
  }

  // Extract features
  float features[NUM_FEATURES];
  extractFeatures(features);
  
  // Debug: Print raw features
  Serial.println("Raw features:");
  for (int i = 0; i < NUM_FEATURES; i++) {
    Serial.print(features[i], 4);
    Serial.print(" ");
    if ((i + 1) % 4 == 0) Serial.println();
  }
  
  normalizeFeatures(features);
  
  // Debug: Print normalized features
  Serial.println("Normalized features:");
  for (int i = 0; i < NUM_FEATURES; i++) {
    Serial.print(features[i], 4);
    Serial.print(" ");
    if ((i + 1) % 4 == 0) Serial.println();
  }

  // Copy features to input tensor
  for (int i = 0; i < NUM_FEATURES; i++) {
    input->data.f[i] = features[i];
  }

  // Run inference
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    Serial.println("ERROR: Invoke failed");
    samples_read = 0;
    return;
  }

  // Find highest confidence class
  float max_score = output->data.f[0];
  int max_index = 0;
  for (int i = 1; i < NUM_CLASSES; i++) {
    if (output->data.f[i] > max_score) {
      max_score = output->data.f[i];
      max_index = i;
    }
  }

  // Print result with all class scores for debugging
  Serial.print("Detected: ");
  Serial.print(GESTURES[max_index]);
  Serial.print(" (");
  Serial.print(max_score * 100.0, 1);
  Serial.println("%)");
  
  // Print all scores for debugging
  Serial.print("  Scores: ");
  for (int i = 0; i < NUM_CLASSES; i++) {
    Serial.print(GESTURES[i]);
    Serial.print("=");
    Serial.print(output->data.f[i] * 100.0, 1);
    Serial.print("% ");
  }
  Serial.println();

  // Reset for next gesture
  samples_read = 0;
  delay(500);
}
