## Software

In deze map vind je alle software voor dit project. Dit omvat de code voor de auto, de handcontroller en de targets.

### Bestandsstructuur
* **MainCode:** Hierin is de volledige, uiteindelijke broncode te vinden.
* **Test Codes:** Er zijn diverse scripts geschreven om specifieke onderdelen los te testen, zoals de BLE-verbinding of de motoraansturing.

### Gebruikte Libraries
De meeste programma's maken gebruik van standaard Arduino-libraries. Hieronder vind je een overzicht van de belangrijkste:

| Library | Functie |
| :--- | :--- |
| `ArduinoBLE.h` | Voor het opzetten en beheren van de Bluetooth Low Energy (BLE) verbinding. |
| `Servo.h` | Voor het eenvoudig aansturen van de servomotor. |
| `Arduino_BMI270_BMM150.h` | Voor het uitlezen van sensor- en gyroscoopdata. |