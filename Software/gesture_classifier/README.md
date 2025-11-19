# gesture_classifier model

In deze folder zit de gesture_classifier model die gebruikt wordt in de software voor het classificeren van handgebaren waaronder punch en circle. Deze handgebaren worden gebruikt om de tank bepaalde acties te laten uitvoeren. Punch voor het schieten van het kannon en circle voor het veranderen van modus om het kannon te richten of de tank te laten rijden.

## Inhoud van de folder

de belangrijkste bestanden in deze folder zijn:

- [convert_cbor_to_csv.py](convert_cbor_to_csv.py): Script om de CBOR data van de handgebaren om te zetten naar CSV formaat.
- [train_tflite_model.py](train_tflite_model.py): Script om het TensorFlow Lite model te trainen met de CSV data.
- [gesture_classifier.ino](gesture_classifier.ino): Arduino code om het getrainde model te implementeren op een microcontroller.

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

3. Installeer Arduino CLI (als je dit nog niet hebt):

   - Download Arduino CLI van [arduino.cc/en/software](https://arduino.cc/en/software) of installeer via package manager:

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
   arduino-cli lib install "Arduino_TensorFlowLite"
   ```

4. Compile en flash de Arduino code `gesture_classifier.ino` naar je microcontroller met behulp van de Arduino IDE of Arduino CLI.

```bash
arduino-cli compile --fqbn arduino:mbed_nano:nano33ble gesture_classifier
arduino-cli upload -p <PORT> --fqbn arduino:mbed_nano:nano33ble gesture_classifier
```

5. Controleer of het werkt door de seriële monitor te openen in de Arduino IDE of met andere seriële terminal software zoals putty:

```bash
arduino-cli monitor -p <PORT> --fqbn arduino:mbed_nano:nano33ble
```

## Hoe model maken

Om de gesture_classifier model te maken heb ik volgende stappen gevolgd:

1. **Data verzamelen**: Handgebaren data werd verzameld met behulp van een microcontroller en Edge impulse platform.
2. **Data omzetten**: De data die ik via edge impulse had verzameld werd in CBOR formaat opgeslagen. Het script `convert_cbor_to_csv.py` werd gebruikt om deze data om te zetten naar CSV formaat voor verdere verwerking.
3. **Model trainen**: Het script `train_tflite_model.py` werd gebruikt om het TensorFlow Lite model te trainen met de CSV data. Hierbij werden verschillende parameters ingesteld zoals het aantal epochs en de batch size.
4. **Model implementeren**: Het getrainde model werd vervolgens geïmplementeerd op een microcontroller met behulp van de Arduino code in `gesture_classifier.ino`

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

- **TensorFlow Lite initialisatie**: De code includeert de TensorFlow Lite Micro bibliotheek en laadt het model uit `gesture_model.h`. Een interpreter wordt aangemaakt met een tensor arena (memory buffer) voor het uitvoeren van inferentie.

- **IMU data verzameling**: De ingebouwde IMU sensor (BMI270/BMM150 op Nano 33 BLE Sense Rev2) wordt gebruikt om accelerometer en gyroscoop data te lezen. De code verzamelt een window van metingen (bijvoorbeeld 50-100 samples) om één gebaar te representeren.

- **Feature extraction op microcontroller**: Net zoals in het training script worden statistische features (mean, std, min, max) berekend voor elke as van de verzamelde sensor data. Deze features worden genormaliseerd met de opgeslagen `mean` en `std` waarden.

- **Inferentie**: De genormaliseerde features worden in de input tensor van het model geplaatst, waarna `interpreter->Invoke()` wordt aangeroepen om de voorspelling te maken. De output tensor bevat waarschijnlijkheden voor elke klasse.

- **Classificatie en actie**: Het gebaar met de hoogste waarschijnlijkheid (boven een bepaalde threshold) wordt geselecteerd. Op basis van het gedetecteerde gebaar (bv. "punch" of "circle") worden specifieke acties uitgevoerd zoals het sturen van een commando via Bluetooth of het aansturen van outputs.
