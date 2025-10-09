# Handschoen controller

## Concept
Een Arduino Nano 33 BLE Sense Ref2 stuurt de pitch & angle data van de interne accelerometer door, die omgezet worden naar bewegings- commando's voor de RC wagen. Deze Arduino wordt gemonteerd op een handschoen, zodat de user de wagen kan besturen door zijn of haar hand te kantelen naar voren/achteren/links/rechts.
Optimaal wordt de Arduino gemonteerd op een buigzaam stuk kurk.

Op de Arduino zit ook nog een drukknop die kan schakelen tussen twee modes: Wagen besturen, en Turret besturen.

De besturing van de turret op de wagen loopt chronoloog aan de besturing van de wagen:
Links en rechts roteren de wagen op de vaste as van zijn positie.
Vooruit en achteruit roteren een extra servomotor die de IR-laser turret hoger of lager doet mikken.
De IR-laser wordt ook aan/uit getoggled door deze knop.