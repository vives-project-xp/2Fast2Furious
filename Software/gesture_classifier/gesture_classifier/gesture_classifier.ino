
// Uncomment the library definition according to your board version
//#include <Arduino_LSM9DS1.h>      // IMU Sensor Library for Arduino Nano 33 BLE Rev.1
#include <Arduino_BMI270_BMM150.h>  // IMU Sensor Library for Arduino Nano 33 BLE Rev.2

// Include Edge Impulse library
#include <Gesture-Classifier_inferencing.h>

// Settings
const float accelerationThreshold = 2.5; // Threshold (in G values) to detect gesture start
const int numSamples = EI_CLASSIFIER_RAW_SAMPLE_COUNT; // 200 samples from model metadata
const int numAxes = EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME; // 6 axes (3 accel + 3 gyro)

int samplesRead = 0; // Sample counter

// Buffer to store raw sensor data
static float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];

// Labels from the model
const char* GESTURES[] = {
  "Punch",
  "circle", 
  "idle"
};

#define NUM_GESTURES (sizeof(GESTURES) / sizeof(GESTURES[0]))

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Initialize IMU sensor
  if (!IMU.begin()) {
    Serial.println("ERROR: IMU sensor initialization failed!");
    while (true); // Stop program here
  }

  Serial.println("Gesture Classifier Initialized");
}

void loop() {
  float aX, aY, aZ, gX, gY, gZ;

  // Collect samples continuously
  while (samplesRead < numSamples) {
    // Check if both sensors have data available
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
      // Read acceleration and gyroscope values
      IMU.readAcceleration(aX, aY, aZ);
      IMU.readGyroscope(gX, gY, gZ);

      // Store values in the features buffer
      // Format: [aX1, aY1, aZ1, gX1, gY1, gZ1, aX2, aY2, aZ2, gX2, gY2, gZ2, ...]
      features[samplesRead * numAxes + 0] = aX;
      features[samplesRead * numAxes + 1] = aY;
      features[samplesRead * numAxes + 2] = aZ;
      features[samplesRead * numAxes + 3] = gX;
      features[samplesRead * numAxes + 4] = gY;
      features[samplesRead * numAxes + 5] = gZ;
      
      samplesRead++; 

      // When all samples are collected, run inference
      if (samplesRead == numSamples) {
        // Create signal from features buffer
        signal_t signal;
        int err = numpy::signal_from_buffer(features, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
        if (err != 0) {
          Serial.print("ERROR: Failed to create signal from buffer (");
          Serial.print(err);
          Serial.println(")");
          samplesRead = 0; // Reset for next cycle
          return;
        }

        // Run classifier
        ei_impulse_result_t result = { 0 };
        err = run_classifier(&signal, &result, false);
        
        if (err != EI_IMPULSE_OK) {
          Serial.print("ERROR: Failed to run classifier (");
          Serial.print(err);
          Serial.println(")");
          samplesRead = 0; // Reset for next cycle
          return;
        }

        // Find the classification with highest confidence
        float max_confidence = 0;
        const char* predicted_label = "";
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
          if (result.classification[ix].value > max_confidence) {
            max_confidence = result.classification[ix].value;
            predicted_label = result.classification[ix].label;
          }
        }
        
        // Print only the gesture with highest confidence
        Serial.print(predicted_label);
        Serial.print(": ");
        Serial.print(max_confidence * 100.0, 2);
        Serial.println("%");
        
        // Reset sample counter for next inference cycle
        samplesRead = 0;
      }
    }
  }
}
