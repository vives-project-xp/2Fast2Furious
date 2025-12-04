## Software

In deze map bevindt zich alle **broncode** die ontwikkeld is voor dit project.

### Bestandsstructuur

De code is georganiseerd in de volgende hoofdmappen om het beheer en de testbaarheid te optimaliseren:

* **`MainCode/`**: Bevat de **volledige, definitieve broncode** voor de productieversie van de robot.
    * `SpeedControlledCar.ino`: Code voor de **robotwagen** met speed controlled auto. (geupload naar de Arduino op de auto).
    * `Hand.ino`: Code voor de **handcontroller** (geupload naar de Arduino van de controller).
    * `gesture_classifier.ino`: Code voor de **handcontroller** met gestures als **Circle** en **Punch** om van mode te switchen en te "schieten". [Voor meer info hier over klik hier](./MainCode/gestureModel/README.md) .


* **`Test Codes/`**: Bevat diverse **losse scripts** die zijn geschreven om specifieke hardware- of software-onderdelen onafhankelijk te testen, zoals de BLE-verbinding, motoraansturing of sensordata-uitlezing.

### Gebruikte Libraries
De meeste programma's maken gebruik van standaard Arduino-libraries. Hieronder vind je een overzicht van de belangrijkste:

| Library | Functie |
| :--- | :--- |
| `ArduinoBLE.h` | Voor het opzetten en beheren van de Bluetooth Low Energy (BLE) verbinding. |
| `Servo.h` | Voor het eenvoudig aansturen van de servomotor. |
| `Arduino_BMI270_BMM150.h` | Voor het uitlezen van sensor- en gyroscoopdata. |
| `Chirale_TensorFlowLite.h` | Voor de gesture classifier. |



### Uploaden & Setup

Om de code te uploaden en het project te kunnen draaien, volgt u de onderstaande stappen:

1.  **Arduino IDE Installeren:** Zorg ervoor dat de nieuwste versie van de **Arduino IDE** is geïnstalleerd.
2.  **Libraries Installeren:** Installeer de libraries die vermeld staan in de tabel (via **Sketch > Library's Inclusief > Library's Beheren...** in de IDE).
3.  **Uploaden naar de Auto:** Open het **`SpeedControlledCar.ino`** bestand en upload deze code naar de Arduino die is gemonteerd op de **robotwagen**.
4.  **Uploaden naar de Controller:** [Voor meer info hier over klik hier](./MainCode/gestureModel/README.md).

>**Tip:** Zorg ervoor dat in de Arduino IDE het juiste **Board** en de juiste **Poort** zijn geselecteerd voordat u de code uploadt.

---

⬅️ [Terug naar overzicht](../README.md#Inhoud)