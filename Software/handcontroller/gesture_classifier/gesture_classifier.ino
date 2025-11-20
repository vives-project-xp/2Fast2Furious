// TensorFlow Lite Micro gesture classifier
// Uses Arduino Nano 33 BLE Rev.2 with BMI270/BMM150 IMU

#include <Arduino_BMI270_BMM150.h>
#include <Chirale_TensorFlowLite.h>
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

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

// Memory arena for TFLite
constexpr int kTensorArenaSize = 60 * 1024;
alignas(16) uint8_t tensor_arena[kTensorArenaSize];

// Buffers for sensor data
float sample_buffer[NUM_SAMPLES][NUM_AXES];
int samples_read = 0;
// Non-blocking sampling state
bool samplingActive = false;
unsigned long nextSampleMs = 0;
const unsigned long sampleIntervalMs = 10; // ms between samples
unsigned long nextCycleMs = 0; // cooldown between gesture cycles
const unsigned long cycleCooldownMs = 500; // wait after inference before next sampling run
// Logging control: how many xi/yi samples to print per full buffer
const int LOG_SAMPLES = 5; // number of samples to print per cycle (approx)
const int LOG_INTERVAL = (NUM_SAMPLES + LOG_SAMPLES - 1) / LOG_SAMPLES; // ceil division
// Track whether the last detected gesture was idle (index 2 in GESTURES)
bool lastDetectedIsIdle = true;

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

  // Load TFLite model
  model = tflite::GetModel(gesture_model);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("ERROR: Model schema mismatch!");
    while (1);
  }

  // Initialize TFLite interpreter
  static tflite::AllOpsResolver resolver;
  static tflite::MicroInterpreter static_interpreter(
    model, resolver, tensor_arena, kTensorArenaSize);
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

// Replicates getCommand from TEST_CODES/Hand/Hand.ino
String getCommand(int16_t xi, int16_t yi) {
  int singleThreshold = 700;     // Voor enkelvoudige richtingen
  int diagonalThreshold = 300;   // Voor diagonale richtingen

  // Eerst diagonalen checken (lagere drempel)
  // NOTE: xi axis is inverted (forward/backwards swapped)
  if (xi > diagonalThreshold && yi > diagonalThreshold) {
    return "BL"; // was FL
  } else if (xi > diagonalThreshold && yi < -diagonalThreshold) {
    return "BR"; // was FR
  } else if (xi < -diagonalThreshold && yi > diagonalThreshold) {
    return "FL"; // was BL
  } else if (xi < -diagonalThreshold && yi < -diagonalThreshold) {
    return "FR"; // was BR
  }
  // Enkelvoudige richtingen checken (hogere drempel)
  else if (xi > singleThreshold) {
    return "B"; // inverted
  } else if (xi < -singleThreshold) {
    return "F"; // inverted
  } else if (yi > singleThreshold) {
    return "L";
  } else if (yi < -singleThreshold) {
    return "R";
  } 
  else {
    return "S";
  }
}

void loop() {
  float aX, aY, aZ, gX, gY, gZ;
  int16_t xi = 0, yi = 0;
  unsigned long now = millis();

  // Start a new sampling run if not active and cooldown passed
  if (!samplingActive && now >= nextCycleMs) {
    samplingActive = true;
    samples_read = 0;
    nextSampleMs = now; // start immediately
  }

  // While sampling is active, take samples at scheduled intervals (non-blocking)
  if (samplingActive && now >= nextSampleMs) {
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
      IMU.readAcceleration(aX, aY, aZ);
      IMU.readGyroscope(gX, gY, gZ);

      // Calculate xi and yi from acceleration (convert to milli-g)
      xi = (int16_t)(aX * 1000);
      yi = (int16_t)(aY * 1000);

      // Log xi/yi for a subset of samples to avoid flooding serial output
      // This logs approximately LOG_SAMPLES values spread across NUM_SAMPLES
      if ((samples_read % LOG_INTERVAL) == 0) {
        // Only log commands when the last detected gesture was idle
        if (lastDetectedIsIdle) {
          String cmd = getCommand(xi, yi);
          Serial.println(cmd);
        }
      }

      // Store converted values into the sample buffer (as before)
      sample_buffer[samples_read][0] = aX * 9.80665;
      sample_buffer[samples_read][1] = aY * 9.80665;
      sample_buffer[samples_read][2] = aZ * 9.80665;
      sample_buffer[samples_read][3] = gX;
      sample_buffer[samples_read][4] = gY;
      sample_buffer[samples_read][5] = gZ;

      samples_read++;
      nextSampleMs += sampleIntervalMs; // schedule next sample
    } else {
      // IMU data not ready yet — try again shortly
      nextSampleMs = now + 1;
    }
  }

  // If a full buffer has been collected, process it (non-blocking across loop iterations)
  if (samples_read >= NUM_SAMPLES) {
    float features[NUM_FEATURES];
    extractFeatures(features);
    normalizeFeatures(features);

    // Copy features to input tensor
    for (int i = 0; i < NUM_FEATURES; i++) {
      input->data.f[i] = features[i];
    }

    // Run inference
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
      Serial.println("ERROR: Invoke failed");
      // Prepare for next run
      samples_read = 0;
      samplingActive = false;
      nextCycleMs = millis() + cycleCooldownMs;
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

    // Print result with confidence
    Serial.print("Detected: ");
    Serial.print(GESTURES[max_index]);
    Serial.print(" (");
    Serial.print(max_score * 100.0, 1);
    Serial.println("%)");

    // Prepare for next gesture: enter cooldown and stop sampling until cooldown expires
    samples_read = 0;
    samplingActive = false;
    // Update lastDetectedIsIdle so per-sample logging only happens after an idle detection
    lastDetectedIsIdle = (max_index == 2);
    nextCycleMs = millis() + cycleCooldownMs;
  }
}
