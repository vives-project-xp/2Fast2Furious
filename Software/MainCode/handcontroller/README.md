# De software van de handschoen

Deze map bevat alle software die op de Arduino Nano 33 BLE Sense Rev2 van de handschoen draait.

## Bestanden

- `gesture_classifier.ino`: Dit is het hoofdprogramma dat de sensorgegevens leest, gebarenclassificatie uitvoert en de resultaten verzendt via Bluetooth. Deze map bevindt zich in de gesture_classifier map.
- gesture_model.h: Dit bestand bevat het getrainde TensorFlow Lite model dat wordt gebruikt voor gebarenherkenning. Dit bestand moet worden gegenereerd door het trainingsproces en in dezelfde map als `gesture_classifier.ino` worden geplaatst. Het trainen bevindt zich in de [gesture_classifier](../gesture_classifier/README.md) map.

## Vereisten

**Arduino Bibliotheken**:

- `Chirale_TensorFlowLite`
- `Arduino_BMI270_BMM150`
- `ArduinoBLE`

**Model**
Het model `gesture_model.h` moet zich in dezelfde map bevinden als `gesture_classifier.ino`.

## Gebruik

1. je maakt een .venv aan:

```bash
python -m venv .venv
.venv\Scripts\activate   
# Op Linux gebruik je: source .venv/bin/activate
```

2. Installeer de benodigde packages:

```bash
pip install -r requirements.txt
```

3. Installeer arduino libraries in arduino IDE of via Arduino CLI:

    **Arduino IDE**:
    Installeer volgende libraries in de Arduino IDE via Library Manager (Sketch -> Include Library -> Manage Libraries...):

    - Arduino_BMI270_BMM150
    - Chirale_TensorFlowLite

    **Arduino CLI**:

   - Download Arduino CLI van [arduino.cc/en/software](https://arduino.cc/en/software) of installeer via package manager indien je deze niet hebt:

   ```bash
   # Windows (via winget)
   winget install ArduinoSA.CLI

   # Windows (via chocolatey)
   choco install arduino-cli

   # macOS
   brew install arduino-cli

   # Linux
   curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
   ```

   - Installeer de benodigde boards en libraries:

   ```bash
   arduino-cli core update-index
   arduino-cli core install arduino:mbed_nano
   arduino-cli lib install "Arduino_BMI270_BMM150"
   arduino-cli lib install "Chirale_TensorFlowLite"
   ```

4. Compile en flash de Arduino code `gesture_classifier.ino` naar je microcontroller met behulp van de Arduino IDE of Arduino CLI.

```bash
cd handcontroller/gesture_classifier
arduino-cli compile --fqbn arduino:mbed_nano:nano33ble gesture_classifier
arduino-cli upload -p <COM PORT> --fqbn arduino:mbed_nano:nano33ble gesture_classifier
```

vervang `<COM PORT>` door de juiste seriële poort van je microcontroller.

5. Controleer of het werkt door de seriële monitor te openen in de Arduino IDE of met andere seriële terminal software zoals putty:

```bash
arduino-cli monitor -p <COM PORT> --fqbn arduino:mbed_nano:nano33ble
```

Verander `<COM PORT>` naar de juiste seriële poort van je microcontroller.

## Uitleg van de code

### Overzicht

De `gesture_classifier.ino` code combineert IMU-sensordata met machine learning om handgebaren te herkennen en te classificeren. De code werkt non-blocking, wat betekent dat het geen gebruik maakt van `delay()` functies die het programma blokkeren, waardoor BLE-communicatie en andere taken soepel blijven verlopen.

### Hoofdcomponenten

#### 1. Globale Configuratie en Constanten

```cpp
const int NUM_SAMPLES = 200;      // Aantal samples per gebaar
const int NUM_AXES = 6;           // 3 accelerometer + 3 gyroscoop assen
const int NUM_FEATURES = 24;      // 4 statistieken per as (mean, std, min, max)
const int NUM_CLASSES = 3;        // Aantal gebaar-klassen
```

- **NUM_SAMPLES**: Er worden 200 datapunten verzameld voordat een gebaar wordt geclassificeerd
- **NUM_AXES**: 6 dimensies van bewegingsdata (X, Y, Z voor acceleratie en rotatie)
- **NUM_FEATURES**: Per as worden 4 statistieken berekend (gemiddelde, standaarddeviatie, minimum, maximum)
- **GESTURES[]**: Array met de namen van de gebaren: "Punch", "circle" en "idle"

#### 2. TensorFlow Lite Setup

```cpp
constexpr int kTensorArenaSize = 12 * 1024;  // 12KB voor TensorFlow
alignas(16) uint8_t tensor_arena[kTensorArenaSize];
```

De TensorFlow Lite interpreter heeft geheugen nodig om het neural network model uit te voeren. Dit wordt gereserveerd in de `tensor_arena` buffer van 12KB.

#### 3. Data Opslag en Sampling

```cpp
int16_t sample_buffer[NUM_SAMPLES][NUM_AXES];
```

De sensordata wordt opgeslagen als 16-bit integers om geheugen te besparen:

- **Acceleratie**: opgeslagen als milli-g (waarde × 1000)
- **Gyroscoop**: opgeslagen als centi-graden/sec (waarde × 100)

Dit bespaart 50% geheugen vergeleken met 32-bit floats, wat cruciaal is op een microcontroller.

#### 4. Non-Blocking Sampling State Machine

```cpp
bool samplingActive = false;
unsigned long nextSampleMs = 0;
const unsigned long sampleIntervalMs = 10;        // 10ms tussen samples
const unsigned long cycleCooldownMs = 500;        // 500ms pauze na detectie
```

De code gebruikt een state machine met tijdstempels om non-blocking te werken:

- Samples worden elke 10ms genomen (100 Hz)
- Na detectie van een gebaar volgt een cooldown van 500ms
- Dit voorkomt dat dezelfde beweging meerdere keren wordt gedetecteerd

#### 5. Preprocessing Parameters

```cpp
const float feature_mean[NUM_FEATURES] = { ... };
const float feature_std[NUM_FEATURES] = { ... };
```

Deze arrays bevatten de normalisatieparameters die tijdens het trainen van het model zijn berekend. Ze worden gebruikt om de features te standaardiseren (zero mean, unit variance), wat essentieel is voor accurate classificatie.

### Setup() Functie

De `setup()` functie initialiseert alle componenten:

1. **IMU Initialisatie**

   ```cpp
   if (!IMU.begin()) {
     Serial.println("ERROR: IMU initialization failed!");
     while (1);
   }
   ```

  Start de BMI270/BMM150 sensor en controleert of deze correct werkt.

2. **BLE Setup**

   ```cpp
   BLE.setLocalName("NanoClassifier");
   BLE.setAdvertisedService(controlService);
   ```

   Configureert Bluetooth Low Energy met de naam "NanoClassifier" en maakt een service aan met UUID `12345678-1234-5678-1234-56789abcdef0`.

3. **TensorFlow Lite Model Laden**

   ```cpp
   model = tflite::GetModel(gesture_model);
   static tflite::MicroInterpreter static_interpreter(...);
   ```

   Laadt het getrainde model uit `gesture_model.h` en initialiseert de TensorFlow Lite interpreter.

### Loop() Functie - Hoofdlogica

De `loop()` functie doorloopt continu verschillende fases:

#### Fase 1: BLE Polling en Connectie Management

```cpp
BLE.poll();
if (BLE.connected() && !deviceConnected) {
  Serial.println("BLE connected");
  deviceConnected = true;
}
```

Houdt de BLE-verbinding actief en detecteert wanneer een apparaat verbindt/disconnecteert.

#### Fase 2: Start van Sampling Cyclus

```cpp
if (!samplingActive && now >= nextCycleMs) {
  samplingActive = true;
  samples_read = 0;
  nextSampleMs = now;
}
```

Start een nieuwe sampling cyclus wanneer:

- Er momenteel geen sampling actief is
- De cooldown periode is verlopen

#### Fase 3: Data Acquisitie

```cpp
if (samplingActive && now >= nextSampleMs) {
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(aX, aY, aZ);
    IMU.readGyroscope(gX, gY, gZ);
    
    // Opslaan in buffer als int16_t
    sample_buffer[samples_read][0] = (int16_t)(aX * 1000.0f);
    // ... etc voor andere assen
  }
}
```

Leest IMU-data wanneer beschikbaar en slaat deze compact op in de sample buffer.

#### Fase 4: Realtime Bewegingscommando's

```cpp
if ((samples_read % LOG_INTERVAL) == 0 && lastDetectedIsIdle) {
  String cmd = getCommand(xi, yi);
  Serial.println(cmd);
  commandCharacteristic.writeValue(cmd);
}
```

Tijdens het verzamelen van samples worden bewegingsrichtingen berekend op basis van de acceleratiedata:

- **F**: Forward (vooruit)
- **B**: Backward (achteruit)
- **L**: Left (links)
- **R**: Right (rechts)
- **FL, FR, BL, BR**: Diagonale richtingen
- **S**: Stop (stilstand)

Deze commando's worden alleen verstuurd wanneer het laatste gedetecteerde gebaar "idle" was, om interferentie met gebaar-detectie te voorkomen.

#### Fase 5: Feature Extraction

```cpp
void extractFeatures(float features[NUM_FEATURES]) {
  for (int axis = 0; axis < NUM_AXES; axis++) {
    // Bereken mean, std, min, max voor elke as
    float mean = sum / NUM_SAMPLES;
    float std_dev = sqrt(variance);
    features[base_idx + 0] = mean;
    features[base_idx + 1] = std_dev;
    features[base_idx + 2] = min_val;
    features[base_idx + 3] = max_val;
  }
}
```

Voor elke van de 6 assen worden 4 statistische kenmerken berekend:

- **Gemiddelde**: centrale tendentie van de beweging
- **Standaarddeviatie**: variabiliteit/intensiteit van de beweging
- **Minimum**: extreme waarde in negatieve richting
- **Maximum**: extreme waarde in positieve richting

Dit resulteert in 24 features (6 assen × 4 statistieken).

#### Fase 6: Feature Normalisatie

```cpp
void normalizeFeatures(float features[NUM_FEATURES]) {
  for (int i = 0; i < NUM_FEATURES; i++) {
    features[i] = (features[i] - feature_mean[i]) / feature_std[i];
  }
}
```

Standaardiseert alle features naar gemiddelde 0 en standaarddeviatie 1. Dit is cruciaal omdat:

- Het neural network verwacht genormaliseerde input
- Het voorkomt dat features met grotere waarden domineren
- Het verbetert de trainingssnelheid en nauwkeurigheid

#### Fase 7: Neural Network Inferentie

```cpp
for (int i = 0; i < NUM_FEATURES; i++) {
  input->data.f[i] = features[i];
}
interpreter->Invoke();
```

De genormaliseerde features worden naar de input tensor van het model gekopieerd, waarna `Invoke()` het neural network uitvoert en voorspellingen genereert.

#### Fase 8: Classificatie en Output

```cpp
float max_score = output->data.f[0];
int max_index = 0;
for (int i = 1; i < NUM_CLASSES; i++) {
  if (output->data.f[i] > max_score) {
    max_score = output->data.f[i];
    max_index = i;
  }
}
```

Het model output bevat een confidence score voor elke klasse. De klasse met de hoogste score wordt geselecteerd:

- **Index 0 (Punch)**: Stuurt commando "P"
- **Index 1 (Circle)**: Stuurt commando "C"
- **Index 2 (Idle)**: Geen speciale actie, activeert bewegingscommando's

#### Fase 9: Cooldown en State Reset

```cpp
samples_read = 0;
samplingActive = false;
lastDetectedIsIdle = (max_index == 2);
nextCycleMs = millis() + cycleCooldownMs;
```

Na classificatie wordt de state gereset en begint een cooldown periode van 500ms voordat de volgende detectiecyclus start.
