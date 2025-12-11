# Handleiding

Hier kan je een handleiding vinden voor het in elkaar steken en werkende te krijgen van de tank.

## tank bouwen

Maak eerst de tank volgens het elektrische schema in de map [hardware](../Hardware/README.md).

![Elektrisch schema tank](../Hardware/Auto/Schema_Auto_SpeedControlled_1Pin.png)

Bevestig daarna alle onderdelen op de grondplaat van de tank. De stl files voor deze af te printen vind je in de map [hardware/tank/STL_files](../Hardware/Tank/STL_files/README.md).

## Software installeren op de tank

Op de microcontroller van de tank moet je [speedcontrolledcar.ino](../Software/MainCode/SpeedControlledCar/SpeedControlledCar.ino) uploaden. Dit kan je doen met de Arduino IDE.

Zorg dat je de juiste board en poort selecteert in de Arduino IDE voordat je upload en ook dat je de benodigde libraries hebt geïnstalleerd.
De benodigde libraries zijn:

- Servo
- ArduinoBLE

## Software installeren op de controller

Op de microcontroller van de controller moet je [gesture_classifier.ino](../Software/MainCode/handcontroller/gesture_classifier/gesture_classifier.ino) uploaden. Dit kan je doen met de Arduino IDE.
Zorg dat je de juiste board en poort selecteert in de Arduino IDE voordat je upload en ook dat je de benodigde libraries hebt geïnstalleerd.
De benodigde libraries zijn:

- ArduinoBLE
- Arduino_BMI270_BMM150
- Chirale_TensorFlowLite

Zorg er ook voor dat het model bestand `gesture_model.h` in dezelfde map staat als `gesture_classifier.ino`.

## handcontroller bouwen

Maak de handcontroller volgens het elektrische schema in de map [hardware](../Hardware/README.md).

![Elektrisch schema handcontroller](../Hardware/HandController/handControllerCircuitv2.png)

Bevestig daarna alle onderdelen op de handschoen. De stl files voor deze af te printen vind je in de map [hardware/handschoen/STL_files](../Hardware/HandController/STL_files/README.md).

## model maken

Om een nieuw model te maken voor de handcontroller, volg de stappen in de map [gestureModel](../Software/MainCode/gestureModel/README.md). Daar wordt uitgelegd hoe je een nieuw model kan trainen en exporteren naar een bestand dat je kan gebruiken in de Arduino IDE.

## Verbinden en gebruiken

Zorg dat de tank en de controller beide aanstaan. De controller zal automatisch verbinding maken met de tank zodra deze binnen bereik is. De connectie kun je zien in de seriële monitor van de Arduino IDE of putty op baudrate 115200. Zodra de verbinding is gemaakt, kun je de tank besturen met gebaren en handbewegingen. Als hij geen verbinding maakt, reset de controller op de tank.
