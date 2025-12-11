# Hardware

Deze map bevat alle hardware componenten en schema's voor het project.

## Inhoud

1. [De hardware van de auto](#de-hardware-van-de-auto)
2. [Hand Controller](#hand-controller)
3. [Tank](#tank)
3. [Target](#target)

## De hardware van de auto

### Auto

1. Arduino Nano 33 BLE Sense Rev2
2. 2x L298N motor driver
3. 4x TT-Motor
4. 7.4V Traxxas Batterij
5. Servo SG90 Micro 
6. Breadboard

### Tank gedeelte

1. Servo SG90 Micro
2. Laser Diode Transmitter


### Elektrisch schema

![schema speedcontroller](./Auto/Schema_Auto_SpeedControlled_1Pin.png)

*Figuur: Elektrisch schema van de auto met Arduino Nano 33 BLE Sense REV2: Variabele snelheidsregeling*


Om bovenstaande schema na te maken moet je de twee omhulsels van de buitenste connector pinnen op de motordrivers verwijderen. Zie foto hieronder.

![schema speedcontroller](./Auto/H_Brug.png)

#### Variabele snelheidsregeling: 
Op het schema zijn alle motoren aangesloten op 1 snelheids regeling Pin. Dit zorgt er voor dat je alle motoren samen kunt versnellen of vertragen. Dit word bij ons gebruikt om in "tank mode" rond te draaien zodat de tank niet te snel draait en in de "car mode" om de snelhied iets te berpeken tijdens het rijden.

### Schema Laser 

Helaas stuurt de Arduino Nano 33 BLE Rev2 maar 3,3V signaalen uit en heeft de laser zelf 5V nodig. Met 3,3V is deze laser niet fel genoeg. Hiervoor moesten we nog een schakeling voorzien.

Deze schakeling bestaat uit volgende onderdelen:
1. HW-493 Laser board
2. BD135 NPN Transistor
3. 220 Ohm weerstand
4. 3xAAA batterijhouder

De drie AAA-batterijen (4,5 V) voeden de laser zodat deze feller is. De Arduino gebruikt de transistor als een digitale schakelaar om deze laser AAN en UIT te zetten.

![schema laser](./Auto/schema%20laser.png)

## Hand Controller

### Concept

De hand controller is de intuïtieve/immersive interface voor de besturing van de RC-car, met uitbreiding van een turret.

### Kern

De controller maakt gebruik van een Arduino Nano 33 BLE Sense Rev2. Deze microcontroller werd gekozen vanwege:

1. Inertial Measurement Unit (IMU): De interne accelerometer en gyroscoop lezen de pitch (voor/achterwaartse kanteling) en de roll (zijwaartse kanteling) data om de oriëntatie van de hand in real-time te bepalen.
2. Bluetooth Low Energy (BLE) Module: Voor draadloze communicatie met de RC-car.
3. Magic Wand/Tensorflow Gesture Recognition als uitbreiding. Laat toe te schakelen tussen verschillende modussen of functies van de controller zonder extra knoppen te monteren/de andere hand te moeten gebruiken.

### Montage

De Arduino wordt gemonteerd op een 3D-geprintte palm-bracelet dat ook de 3x AAA batterijhouder power supply huist. Zie de .stl-files voor meer info.

### Besturing

De bewegingsdata van de IMU wordt in onze software vertaald naar digitale bewegingscommando's en verzonden naar de RC-car.

| Modus | IMU Data | Commando |
| :--- | :--- | :--- |
| Car | Kantelen naar voren | Rijden vooruit |
| Car | Kantelen naar achteren | Rijden achteruit |
| Car | Kantelen naar links | Rijden links |
| Car | Kantelen naar rechts | Rijden rechts |
| Car | Kantelen naar linksvoor | Rijden linksvoor in hondengang |
| Car | Kantelen naar rechtsvoor | Rijden rechtsvoor in hondengang |
| Car | Kantelen naar linksachter | Rijden linksachter in hondengang |
| Car | Kantelen naar rechtsachter | Rijden rechtsachter in hondengang |
| Turret | Kantelen naar links | Car roteert op as links |
| Turret | Kantelen naar rechts | Car roteert op as rechts |
| Turret | Kantelen naar voren | Turret mikt hoger (servo) |
| Turret | Kantelen naar achteren | Turret mikt lager (servo) |

### Turret Modus

Aan de hand van Gesture Recognition kan de user schakelen tussen verschillende modussen.
In Turret Modus veranderd de interpretatie van de IMU-data:

Zijwaartse kanteling wordt gebruikt om de car op zijn vaste as te laten roteren, zoals in tank-besturing.
De voor- en achterwaartse kanteling wordt gebruikt om een servomotor in de turret aan te sturen. Deze servo is verantwoordelijk voor de elevatie van een gemonteerde laserdiode.

### Laserdiode

De eerdervernoemde Gesture Recognition die zorgt voor schakelen tussen de twee modussen en heeft een secundaire functie, namelijk de laserdiode aan of uit schakelen.
De 'Gesture' die men daarvoor moet doen is een 'punch' of met andere woorden een stoot geven met de vuist.
Dit voorkomt dat de laser onbedoeld aanblijft.


### Onderdelen Hand controller
1. Arduino Nano 33 BLE Sense REV2

2. 3xAAA batterijhouder

3. Bracelet behuizing en straps

#### Schema:

![Schema van de hand controller ](./Handschoen/handControllerCircuitv2.png)

### STL

Alle STL files voor het ontwerp van de handcontroller die gebruikt werden tijdens dit project zijn te vinden in deze [map](./Handschoen/STL_files/).

## Tank

### Previews

De RC tank is gemaakt uit vier verschillende onderdelen, zijnde het chassis van de tank, de carosserie, de kop en een loop. 

Het chassis is gedesigned zodat er plaats voorzien is voor de vier motoren, alsook plaats voor twee driver boards. Verder zijn er kleine pinnen voorzien op het chassis zodat de carosserie er op bevestigd kan worden.

Op de bovenkant van de carosserie is er plaats voorzien voor de servo-motor met ook een gat voor bekabeling. Daarnaast zijn er gaten aan de onderkant zodat de carosserie op het chassis past. Aan de bovenkant zijn er gaten voorzien voor het bevestigen van de kop.

In de kop is er een gat voorzien om de loop door te steken.
Ook zijn er pinnen voorzien aan de onderkant voor de montage op de carosserie.

<img src="../Documentatie/Afbeeldingen/preview_chassis_carosserie.png" alt="chassis" width="400">
<img src="../Documentatie/Afbeeldingen/preview_kop.png" alt="kop" width="400">
<img src="../Documentatie/Afbeeldingen/preview_loop.png" alt="loop" width="400">

### STL

Alle STL files voor het ontwerp van de tank die gebruikt werden tijdens dit project zijn te vinden in deze [map](./Tank/STL_files/).

⬅️ [Terug naar overzicht](../README.md#Inhoud)

