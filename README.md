# GPS tracker on Ham radio APRS network with DRA818 module

## The projet

Inspiration : https://github.com/Toni43/MiniSmartTracker

### Hardware parts used
- Arduino [Nano](https://store.arduino.cc/arduino-nano)
- GPS Module ([NEO 6M](https://www.amazon.fr/NEO-6M-GPS-module-puissance-GY-GPS6MV2/dp/B01ICYA4QU))
- VHF TX module ([DRA818V](http://www.dorji.com/docs/data/DRA818V.pdf))

### Software parts used
- [PlatformIO](https://platformio.org/)
- [QAPRS lib](https://bitbucket.org/Qyon/arduinoqaprs/src/master/) from Lukasz SQ5RWU
- [TinyGPS+ lib](http://arduiniana.org/libraries/tinygpsplus/)
- [DRA818 lib](https://github.com/fatpat/arduino-dra818) from Jereme Loyet

### The circuit

![The circuit](.github/circuit.png "The circuit")

### The Arduino program

The project has one main function : **send its GPS coordinate on ham radio APRS network**.
To accomplish that it has 3 ways :
- With the DRA818V module
- With a Baofeng UV5-R/UVB5 etc connected to the board
- With an external TX by the jack connector

### Compile and run

I use PlatformIO insted of the Arduino IDE.

1. Install [PlatformIO](https://docs.platformio.org/en/latest/installation.html)
2. Run `git clone https://github.com/valentintintin/arduino-tracker-aprs-dra818.git && cd arduino-tracker-aprs-dra818`
3. You have to uncomment `APRS_HW_TYPE_R2R` and comment all the others in `.piolibdeps/ArduinoQAPRS/ArduinoQAPRS.h`
4. Plug your Arduino nano
5. Run `pio run --target upload -e nanoprod`

##### Notes

- There are 2 environments : `nanoprod` without any logs, `nanotest` or with log and APRS sended every time even if no GPS locked.
- The QAPRS lib use a **R2R resistor network** to generate the sound signal. 
- If you want to use only PWM pin you can do it with (look at this : https://github.com/handiko/Arduino-APRS#afsk-bit-banging). You'll have to implement the ax25 protocol. PR welcome :)
- There are many board for DRA818  ([by Handiko](https://github.com/handiko/Dorji-TX-Shield), [by SV1AFN](https://www.sv1afn.com/dra818.html), [by HamShop.cz](https://www.hamshop.cz/avr-arduino-raspberry-pi-c16/vhf-transceiver-module-134174mhz-1w-dra818v-i266/)).
- In boards, Low Pass filter is not always included, **it's recommended to add one** :)

#### Program flow

1. At the startup, the Arduino try to detect if there is a DRA818 module plugged. If the connexion failed, it goes to the Baofeng mode.
*The sound (AX25) is always present on the jack connector and on Baofeng header pins. We don't have to use multiple options at the same time because sound "intensity" will decrease.*
2. If the GPS is locked AND time between TX is reached ==> send the new location. The time between TX is defined by the speed and two affine functions.

### Photos

| | | |
|:-------------------------:|:-------------------------:|:-------------------------:|
|<img width="1604" alt="Arduino circuit" src=".github/arduino.jpg">  |  <img width="1604" alt="DRA818V circuit" src=".github/dra.jpg">|<img width="1604" alt="Arduino and DRA818 circuit back" src=".github/arduino-dra.jpg">

## The author

My name is Valentin and my callsign is F4HVV.
I'm not good in electronics but I can do such project working approximately :)
