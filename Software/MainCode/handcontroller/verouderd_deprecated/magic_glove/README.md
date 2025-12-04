Magic Glove - simplified circle detector

This folder contains a simplified, modernized copy of the Magic Wand logic focused on detecting when
an Arduino Nano 33 BLE Sense (LSM9DS1) attached to a wand draws a circle.

Files
- magic_glove.ino  : Arduino sketch (main)

How it works
- The sketch reads gyroscope samples, integrates them into a simple orientation estimate
  (degrees) and collects orientation samples while the wand is moving.
- When motion stops the collected points are analyzed for circularity by computing
  the centroid, radius consistency, and angular coverage. If the stroke covers roughly
  a full revolution with consistent radius, a circle is reported.

Requirements
- Arduino Nano 33 BLE Sense (or another board with LSM9DS1 compatible IMU and the
  Arduino_LSM9DS1 library).
- Arduino IDE or Arduino CLI. The sketch uses only the Arduino built-in APIs and
  the Arduino_LSM9DS1 library.

How to upload
1. Open `magic_glove.ino` in the Arduino IDE.
2. Install the "Arduino Nano 33 BLE" board support and the `Arduino_LSM9DS1` library
   if you don't already have them.
3. Select the board and port, then press Upload.
4. Open the Serial Monitor at 115200 baud and follow the printed messages.

Notes & next steps
- This example deliberately does not include a TensorFlow model; the geometric
  approach is lightweight and works without ML. If you want to run a model for
  more complex gestures you can add TensorFlow Lite for Microcontrollers later.
- For improved accuracy use sensor fusion (Madgwick/Mahony) or add a simple
  low-pass filter / drift estimation for gyroscope bias removal.
