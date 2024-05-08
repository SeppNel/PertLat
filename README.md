# PertLat - Latency Tester
![photo](https://github.com/SeppNel/PertLat/assets/35899928/631d7f13-a800-4427-935c-2a0b26d6c5b3)

Code for building a latency tester compatible with PC and Gamecube with an arduino.
Also works with the switch with some GC adapters.

## Software Requirements
- [Arduino IDE](https://www.arduino.cc/en/Main/Software)
- [Arduino Nintendo Library](https://github.com/NicoHood/Nintendo) by NicoHood
- [PertFlash](https://github.com/SeppNel/PertFlash) - My client app for receiving the results in real time and do the test on PC.

## Hardware

### Parts
- Logic Level Converter 5v <-> 3.3v
- Arduino Nano (I used a cheap clone)
- 1kΩ Resistor
- Female USB port
- Male USB to Male Gamecube controller connector cable

### Wiring
![Wiring diagram](https://github.com/SeppNel/PertLat/assets/35899928/54fa1103-dab2-4cd5-8654-cddbfa17954c)

The colors of the wires in the cables are orientative, don't assume your GC extensions are color coded the same.

#### GameCube Pinout
Type	| Original Controller
--------|-----------
gnd		| Black
gnd		| white
gnd		| green
data	| red
5v		| yellow
3.3v	| blue
