# gesture_classifier model

In deze folder zit de gesture_classifier model die gebruikt wordt in de software voor het classificeren van handgebaren waaronder punch en circle. Deze handgebaren worden gebruikt om de tank bepaalde acties te laten uitvoeren. Punch voor het schieten van het kannon en circle voor het veranderen van modus om het kannon te richten of de tank te laten rijden.

## Inhoud van de folder

de belangrijkste bestanden in deze folder zijn:

- [convert_cbor_to_csv.py](convert_cbor_to_csv.py): Script om de CBOR data van de handgebaren om te zetten naar CSV formaat.
- [train_tflite_model.py](train_tflite_model.py): Script om het TensorFlow Lite model te trainen met de CSV data.
- [gesture_classifier.ino](gesture_classifier.ino): Arduino code om het getrainde model te implementeren op een microcontroller. Deze bevindt zich in de [gesture_classifier](./gesture_classifier/gesture_classifier.ino) map van de handcontroller software.

Dit zijn allemaal codebestanden die gebruikt worden om het model te maken en te implementeren. Daarnaast zijn er ook enkele gegenereerde bestanden:

- `dataset.csv`: De gegenereerde dataset in CSV formaat na het uitvoeren van `convert_cbor_to_csv.py`. Gebruikt als input voor het trainingsscript.
- `gesture_model.tflite`: Het getrainde TensorFlow Lite model na het uitvoeren van `train_tflite_model.py`.
- `gesture_model.h`: C header file met het model als byte array voor gebruik in Arduino

Deze bestanden worden automatisch gegenereerd door de scripts en hoeven niet handmatig te worden aangepast.

De gesture-classifier-export map bevat de ruwe data die verzameld is via Edge Impulse. Deze data wordt gebruikt als input voor het conversie script. Deze map is niet in de git repository opgenomen vanwege de grootte van de bestanden. Deze map kan je zelf aanmaken door data te verzamelen via Edge Impulse zoals beschreven in de sectie "Hoe model maken" verderop in deze README.

## gebruik

Om het model te gebruiken moet je volgende stappen volgen:

1. je maakt een .venv aan en activeert deze:

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

    Arduino IDE:
    Installeer volgende libraries in de Arduino IDE via Library Manager (Sketch -> Include Library -> Manage Libraries...):
    - Arduino_BMI270_BMM150
    - Chirale_TensorFlowLite

    Arduino CLI:

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
cd gesture_classifier
arduino-cli compile --fqbn arduino:mbed_nano:nano33ble gesture_classifier
arduino-cli upload -p <COM PORT> --fqbn arduino:mbed_nano:nano33ble gesture_classifier
```

vervang `<COM PORT>` door de juiste seriële poort van je microcontroller.

5. Controleer of het werkt door de seriële monitor te openen in de Arduino IDE of met andere seriële terminal software zoals putty:

```bash
arduino-cli monitor -p <COM PORT> --fqbn arduino:mbed_nano:nano33ble
```

Verander `<COM PORT>` naar de juiste seriële poort van je microcontroller.

## Hoe model maken

Om de gesture_classifier model te maken heb ik volgende stappen gevolgd:

### 1. Data verzamelen via Edge Impulse

Data van handgebaren werd verzameld met behulp van een microcontroller en Edge Impulse platform:

- Ga naar [Edge Impulse](https://edgeimpulse.com) en maak een nieuw project aan
- Verbind je Arduino Nano 33 BLE Sense Rev2 met Edge Impulse via de Data Forwarder of Edge Impulse CLI
- Maak labels aan voor elk gebaar (bijvoorbeeld "punch", "circle", "idle")
- Verzamel meerdere samples per gebaar (minimaal 30-50 per klasse, meer is beter)
  - Voer elk gebaar meerdere keren uit met verschillende snelheden en orientaties
  - Zorg voor consistentie in de duur van elk gebaar (bijvoorbeeld 2 seconden)
- Splits je data automatisch of handmatig in training (80%) en testing (20%) sets

### 2. Data exporteren uit Edge Impulse

Nadat je genoeg data hebt verzameld (30 tot 50 per gebaar), exporteer je de dataset:

- In Edge Impulse Studio ga naar "Dashboard" of "Data acquisition"
- Klik op het export icoon of gebruik het menu om "Export" te selecteren
- Kies voor "Export as raw features" of de ingestion data optie
- Download de ZIP file en pak deze uit

De uitgepakte folder structuur moet er als volgt uitzien:

```folder
gesture-classifier-export/
├── info.labels                          # JSON file met label mapping
├── training/                            # Training data
│   ├── circle.69md0drd.ingestion-...cbor
│   ├── circle.69md0nfj.ingestion-...cbor
│   ├── punch.69me1abc.ingestion-...cbor
│   ├── punch.69me2def.ingestion-...cbor
│   └── ...
└── testing/                             # Testing data
    ├── circle.69me5pve.ingestion-...cbor
    ├── punch.69me6ov4.ingestion-...cbor
    └── ...
```

Het `info.labels` bestand bevat de mapping tussen bestandsnamen en labels:

```json
{
  "version": 1,
  "files": [
    {
      "name": "circle.69md0drd",
      "label": {
        "type": "label",
        "label": "circle"
      }
    },
    {
      "name": "punch.69me1abc",
      "label": {
        "type": "label",
        "label": "punch"
      }
    }
  ]
}
```

### 3. Data omzetten naar CSV

We kunnen de CBOR bestanden niet direct gebruiken om het model te trainen, dus moeten we ze eerst omzetten naar een CSV bestand.
Gebruik het `convert_cbor_to_csv.py` script om de CBOR bestanden om te zetten naar een CSV bestand:

```bash
python convert_cbor_to_csv.py
```

Het script:

- Leest automatisch de `gesture-classifier-export/` folder
- Gebruikt `info.labels` om de juiste labels toe te wijzen
- Converteert alle CBOR bestanden naar een enkele `dataset.csv`
- De CSV bevat kolommen: `axis0`, `axis1`, ..., `axis5` (voor 6-axis IMU data), `label`, `filename`, `timestep`, `split`

Controleer de output:

```bash
✅ Saved dataset.csv with shape: (15234, 10)
```

### 4. Model trainen

Train het TensorFlow Lite model met het `train_tflite_model.py` script:

```bash
python train_tflite_model.py
```

Het training proces:

- Laadt `dataset.csv` en extraheert statistische features (mean, std, min, max) per opname
- Normaliseert de data en slaat normalisatie parameters op in `model_params.npz`
- Bouwt een neuraal netwerk (input → 32 neurons → 16 neurons → output classes)
- Traint gedurende 50 epochs met een 80/20 train/test split
- Converteert het model naar TensorFlow Lite formaat met optimalisaties
- Genereert een C header file voor Arduino

Output bestanden:

- `gesture_model.tflite` - TensorFlow Lite model (voor Python inferentie of debugging)
- `gesture_model.h` - C header met model als byte array (voor Arduino)
- `labels.json` - Label mapping (bijvoorbeeld `["circle", "punch"]`)
- `model_params.npz` - Normalisatie parameters (mean, std)

Voorbeeld training output:

```bash
Epoch 50/50
12/12 [==============================] - 0s 2ms/step - loss: 0.0234 - accuracy: 0.9896 - val_loss: 0.1123 - val_accuracy: 0.9583

Test Accuracy: 0.9500
✅ Saved gesture_model.tflite
✅ Saved gesture_model.h
```

### 5. Model implementeren op microcontroller

Kopieer de gegenereerde bestanden naar je Arduino project:

- `gesture_model.h` → include in je Arduino sketch
- `labels.json` → (optioneel) om label namen te tonen in plaats van indices
- `model_params.npz` → bevat normalisatie parameters (deze moet je handmatig in code hardcoden)

Implementeer de code in `gesture_classifier.ino`:

- Laad het model uit `gesture_model.h`
- Verzamel IMU data in een window buffer
- Bereken dezelfde statistische features als tijdens training
- Normaliseer met dezelfde mean/std waarden
- Voer inferentie uit en selecteer het gebaar met hoogste waarschijnlijkheid
- Voer acties uit op basis van het gedetecteerde gebaar

### Tips voor betere resultaten

- **Meer data**: Verzamel minimaal 30-50 samples per gebaar voor goede generalisatie
- **Data variatie**: Voer gebaren uit met verschillende snelheden, hoeken en startposities
- **Balanced dataset**: Zorg dat elke klasse ongeveer evenveel samples heeft
- **Idle klasse**: Voeg een "idle" of "unknown" klasse toe voor wanneer geen gebaar wordt uitgevoerd
- **Threshold tuning**: Experimenteer met confidence thresholds (bijv. alleen accepteren als >70% zeker)
- **Real-time testing**: Test het model op de microcontroller met nieuwe gebaren die niet in de training set zaten

## Gebruikte hardware

- microcontroller (arduino nano 33 ble sense rev2)
- IMU sensor (ingebouwd in de microcontroller)

## uitleg code

### convert_cbor_to_csv.py

Dit script leest CBOR bestanden in uit een opgegeven map, converteert de gegevens naar CSV-formaat en slaat ze op in een nieuwe map. Het script maakt gebruik van de `cbor2` bibliotheek om de CBOR-bestanden te decoderen en `pandas` om de gegevens naar CSV te schrijven. Hieruit verkrijg je [dataset.csv](dataset.csv) bestand die gebruikt wordt om het model te trainen.

**Belangrijkste delen van de code:**

- **Label mapping laden**: Het script leest eerst `info.labels` om te bepalen welk label (bv. "circle" of "punch") bij elk bestand hoort. Dit gebeurt door het JSON-bestand te parsen en een dictionary `label_map` aan te maken die bestandsnamen koppelt aan hun labels.

- **CBOR-bestanden inlezen**: Voor elke `.cbor` file in de `training/` en `testing/` mappen wordt het binaire bestand gedecoded met `cbor2.load()`. De functie `find_sensor_matrix()` zoekt recursief door de geneste structuur om de eigenlijke sensor data matrix te vinden (een lijst van lijsten met numerieke waarden).

- **Data flatten en uitbreiden**: Elk tijdstip (timestep) in de sensor matrix wordt omgezet naar een aparte rij in de dataset. Elke rij bevat kolommen voor de verschillende assen (`axis0`, `axis1`, ..., typisch 6 assen voor accelerometer + gyroscoop), plus metadata kolommen (`label`, `filename`, `timestep`, `split`).

- **CSV output**: Alle rijen worden gecombineerd in een Pandas DataFrame en opgeslagen als `dataset.csv`. Dit bestand is gestructureerd en klaar om gebruikt te worden voor machine learning training.

### train_tflite_model.py

Dit script traint een TensorFlow Lite model voor het classificeren van handgebaren op basis van de CSV-gegevens. Het script laadt de gegevens, splitst ze in trainings- en testsets, bouwt een neuraal netwerkmodel, traint het model en slaat het getrainde model op als een TFLite-bestand.

**Belangrijkste delen van de code:**

- **Feature extraction**: In plaats van raw sensor data direct te gebruiken, aggregeert het script per opname (`filename`) statistische kenmerken: voor elke as wordt het gemiddelde, standaarddeviatie, minimum en maximum berekend. Dit reduceert tijdreeksen van variabele lengte naar een vaste vector van features per gebaar.

- **Data preprocessing**: De feature vectors worden genormaliseerd door het gemiddelde af te trekken en te delen door de standaarddeviatie (`X_norm = (X - mean) / std`). Deze preprocessing-parameters (`mean` en `std`) worden opgeslagen in `model_params.npz` zodat dezelfde normalisatie later op de microcontroller kan worden toegepast.

- **Model architectuur**: Een eenvoudig fully-connected neuraal netwerk met twee hidden layers (32 en 16 neurons, ReLU activatie) en een output layer met softmax activatie voor classificatie. Deze architectuur is licht genoeg om op een microcontroller te draaien.

- **Training**: Het model wordt getraind met Adam optimizer en sparse categorical crossentropy loss gedurende 50 epochs. Een deel van de trainingsdata (20%) wordt gebruikt voor validatie tijdens training.

- **TFLite conversie**: Het getrainde Keras model wordt geconverteerd naar TensorFlow Lite formaat met optimalisaties (`tf.lite.Optimize.DEFAULT`) voor kleinere modelgrootte en snellere inferentie op embedded hardware.

- **C array generatie**: Het TFLite model wordt ook geconverteerd naar een C header file (`gesture_model.h`) waar het model als byte array is opgeslagen. Dit maakt het mogelijk om het model direct in Arduino code te embedden zonder een apart bestand te moeten laden.

### gesture_classifier.ino

Dit is de Arduino code die het getrainde TensorFlow Lite model gebruikt om handgebaren te classificeren op een microcontroller. De code initialiseert de benodigde bibliotheken, laadt het model, verzamelt gegevens van de IMU-sensor, voert inferentie uit met het model en neemt acties op basis van de gedetecteerde gebaren.

**Belangrijkste delen van de code:**

- **TensorFlow Lite initialisatie**: De code includeert de Chirale TensorFlow Lite bibliotheek en laadt het model uit `gesture_model.h`. Een interpreter wordt aangemaakt met een tensor arena (memory buffer) voor het uitvoeren van inferentie.

- **IMU data verzameling**: De ingebouwde IMU sensor (BMI270/BMM150 op Nano 33 BLE Sense Rev2) wordt gebruikt om accelerometer en gyroscoop data te lezen. De code verzamelt een window van metingen (bijvoorbeeld 50-100 samples) om één gebaar te representeren.

- **Feature extraction op microcontroller**: Net zoals in het training script worden statistische features (mean, std, min, max) berekend voor elke as van de verzamelde sensor data. Deze features worden genormaliseerd met de opgeslagen `mean` en `std` waarden.

- **Inferentie**: De genormaliseerde features worden in de input tensor van het model geplaatst, waarna `interpreter->Invoke()` wordt aangeroepen om de voorspelling te maken. De output tensor bevat waarschijnlijkheden voor elke klasse.

- **Classificatie en actie**: Het gebaar met de hoogste waarschijnlijkheid (boven een bepaalde threshold) wordt geselecteerd. Op basis van het gedetecteerde gebaar (bv. "punch" of "circle") worden specifieke acties uitgevoerd zoals het sturen van een commando via Bluetooth of het aansturen van outputs.

⬅️ [Terug naar overzicht](../../README.md#bestandsstructuur)