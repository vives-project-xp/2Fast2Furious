# Documentation

Deze map bevat documentatie en specificaties voor het project.

## BOM

Hieronder staat de Bill of Materials (BOM) voor het project:

| Component         | Prijs/stuk | Aantal | Totaalprijs | Opmerking               | Link naar component                |
|-------------------|------------|--------|-------------|-------------------------|------------------------------------|
| Arduino Nano 33 BLE Sense Rev2 with Headers       | €44,15     | 2      | €88,30     | Microcontroller met gyroscoop en bluetooth         | [kiwi](https://www.kiwi-electronics.com/en/arduino-nano-33-ble-sense-rev2-with-headers-11207?srsltid=AfmBOorEKgNR_dinSSKnJzBCy2cwIM_K1HHhxGHojLlitCHDhjyoVGH4) |
| mecanum wielen (2 stuks)    | €13,00    | 2      | €26,00     | Wielen voor omnidirectionele beweging | [otronic](https://www.otronic.nl/nl/mecanum-wiel-omnidirectioneel-wiel-80mm-a-geel-set.html) |
| McNamum wielkoppeling | €4,40     | 4      | €17,60      | Koppeling voor mecanum wielen | [otronic](https://www.otronic.nl/nl/67mm-mcnamum-wielkoppeling-met-m2530-schroef.html) |
| TT-motor | €2,00     | 4      | €8,00      | Motor voor aandrijving van de wielen | [otronic](https://www.otronic.nl/nl/tt-motor-voor-aandrijving-wielen-dubbele-as.html) |
| L298N motor driver | €2,60     | 2      | €5,20      | Motor driver voor het aansturen van de TT-motoren | [otronic](https://www.otronic.nl/nl/l298n-motor-driver-board-rood.html) |
| 9V batterijhouder | €1,50     | 1      | €1,50      | Batterijhouder voor 9V batterij | [otronic](https://www.otronic.nl/nl/9v-batterijhouder-met-aan-uit-tuimelschakelaar-9-v.html) |
| 6V batterijhouder | €0,95     | 2      | €1,90      | Batterijhouder voor 4 AA batterijen (6V) | [otronic](https://www.otronic.nl/nl/4x-aa-batterijhouder-6v.html) |
| servomotor SG90 micro | €4,15     | 1      | €4,15      | Servomotor voor kannon op en neer | [otronic](https://www.otronic.nl/nl/servo-sg90-micro-180-graden.html?source=googlebase&gad_source=1&gad_campaignid=19639985996&gbraid=0AAAAACZK5qt2WCV4nNg-E26CpEE2x9rUz&gclid=CjwKCAjwup3HBhAAEiwA7euZuk2WwIJDgAclXEV5tnT1Aj656PHRDnFzcf_AuOX2mFILzhPvTrnJZBoCsQ8QAvD_BwE) |
| lasertransmitter | €1,20     | 1      | €1,20      | Lasertransmitter voor kannon | [otronic](https://www.otronic.nl/nl/laser-diode-5v-module-rode-laser-650nm-5mw-koperen.html) |
| laser ontvanger | €1,95     | 1      | €1,95      | Laser ontvanger voor target | [otronic](https://www.otronic.nl/nl/5v-ontvanger-module-voor-laser-diode.html?source=googlebase&gad_source=1&gad_campaignid=19639985996&gbraid=0AAAAACZK5qt2WCV4nNg-E26CpEE2x9rUz&gclid=CjwKCAjwup3HBhAAEiwA7euZuiG-O_8Bph6YO7Qr4fJCEoCmUfKjolpxiwBfOWCFe0IP3KCnW2P0xhoCoPUQAvD_BwE) |
| arduino nano | €6,80     | 1      | €6,80      | Microcontroller voor de target | [otronic](https://www.otronic.nl/nl/nano-v3-arduino-compatible-ch340-voorgesoldeerd.html?source=googlebase&gad_source=1&gad_campaignid=19639985996&gbraid=0AAAAACZK5qt2WCV4nNg-E26CpEE2x9rUz&gclid=CjwKCAjwup3HBhAAEiwA7euZuuQCWR5InQhULsq2bEmu5rOgdqvDIouzecjb0gOtY7eFn1gCphPufxoCwbwQAvD_BwE) |

De totaalprijs van alle componenten is: €162,60

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

### Architectuur van de auto

![Architectuur van de auto](./schema's/RC_wagen.drawio.svg)

### Architectuur van de handcontroller

![Architectuur van de handcontroller](./schema's/Handcontroller.drawio.svg)
