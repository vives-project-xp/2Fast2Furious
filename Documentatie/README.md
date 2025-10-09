# Documentation

Deze map bevat documentatie en specificaties voor het project.

## BOM

Hieronder staat de Bill of Materials (BOM) voor het project:

| Component         | Prijs/stuk | Aantal | Totaalprijs | Opmerking               | Link naar component                |
|-------------------|------------|--------|-------------|-------------------------|------------------------------------|
| Arduino Nano 33 BLE Sense Rev2 with Headers       | €44,15     | 2      | €88,30     | Microcontroller met gyroscoop en bluetooth         | [kiwi](https://www.kiwi-electronics.com/en/arduino-nano-33-ble-sense-rev2-with-headers-11207?srsltid=AfmBOorEKgNR_dinSSKnJzBCy2cwIM_K1HHhxGHojLlitCHDhjyoVGH4) |
| mecanum wielen (2 stuks)    | €13,00    | 2      | €26,00     | Wielen voor omnidirectionele beweging | [otronic](https://www.otronic.nl/nl/mecanum-wiel-omnidirectioneel-wiel-80mm-a-geel-set.htmll) |
| McNamum wielkoppeling | €4,40     | 4      | €17,60      | Koppeling voor mecanum wielen | [otronic](https://www.otronic.nl/nl/67mm-mcnamum-wielkoppeling-met-m2530-schroef.html) |
| TT-motor | €2,00     | 4      | €8,00      | Motor voor aandrijving van de wielen | [otronic](https://www.otronic.nl/nl/tt-motor-voor-aandrijving-wielen-dubbele-as.html) |
| L298N motor driver | €2,60     | 2      | €5,20      | Motor driver voor het aansturen van de TT-motoren | [otronic](https://www.otronic.nl/nl/l298n-motor-driver-board-rood.html) |
| 9V batterijhouder | €1,50     | 1      | €1,50      | Batterijhouder voor 9V batterij | [otronic](https://www.otronic.nl/nl/9v-batterijhouder-met-aan-uit-tuimelschakelaar-9-v.html) |
| 6V batterijhouder | €0,95     | 1      | €0,95      | Batterijhouder voor 4 AA batterijen (6V) | [otronic](https://www.otronic.nl/nl/4x-aa-batterijhouder-6v.html) |

De totaalprijs van alle componenten is: € 147,55

## componenten

Hieronder wat meer informatie over de gebruikte componenten:

### Arduino Nano 33 BLE Sense Rev2 with Headers

De Arduino Nano 33 BLE Sense Rev2 is een compacte  microcontroller vergelijkbaar met de standaard Arduino Nano, maar met extra functionaliteit. De microcontroller bevat een ingebouwde gyroscoop dat we gebruiken voor de bewegingsdetectie van de handcontroller. Daarnaast heeft de microcontroller bluetooth functionaliteit waarmee we draadloos kunnen communiceren tussen de handcontroller en de auto.

### TT-motor

De TT-motor is een kleine, goedkope en efficiënte motor die vaak wordt gebruikt in robotica en kleine voertuigen. In ons project gebruiken we de TT-motoren om de mecanum wielen aan te drijven.

## L298N motor driver

De L298N motor driver is een motor driver die we gebruiken om de TT-motoren aan te sturen. De motor driver kan twee DC-motoren tegelijk aansturen en kan zowel de snelheid als de draairichting van de motoren regelen.

## Introductieposter

[Hier](./Introductieposter/introductieposter.md) kan je de introductieposter vinden van ons project.

## Architectuur document

Hieronder staan de architectuur documenten voor het project.

#### Architectuur van de auto

#### Architectuur van de handcontroller
