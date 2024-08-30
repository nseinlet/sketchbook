# MH-Z14A, MH-Z19B CO2 ... Module

This repository contains an Arduino implementation for MH-Z CO2 sensors, including MH-Z14A, MH-Z19B, and MH-Z19C. It aims to consolidate information on these sensors in one place for easy access.

The sensor is available for ~20 bucks at the usual places.

## Implementation

Refer to the provided example for implementation details.

> PPMuart: 602, PPMpwm: 595, Temperature: 23

### Implementation details

```
C ppm = 5000 * (T_high - 2 ms) / (T_high + T_low - 4ms)
```

The implementation primarily draws from this [Arduino forum post](https://forum.arduino.cc/index.php?topic=525459.msg3587557#msg3587557).

## Usage
By default, the PWM range value is set to 5000. You do not need to change anything in the class constructor if the Cppm value is within the expected range of 400-1000. However, if necessary, you can test it with a 2000 range value:

```cpp
#include <MHZ.h>
#define CO2_IN 9
#define MH_Z19_RX 10
#define MH_Z19_TX 11

MHZ co2(MH_Z19_RX, MH_Z19_TX, CO2_IN, MHZ19B); // here the range value is set to 5000 by default (RANGE_5K)
MHZ co2(MH_Z19_RX, MH_Z19_TX, CO2_IN, MHZ19B, RANGE_2K); // here the range value is set to 2000
```

## Resources:

Datasheet (MH-Z19B):
http://www.winsen-sensor.com/d/files/infrared-gas-sensor/mh-z19b-co2-ver1_0.pdf

Datasheet (MH-Z19C):
https://pdf1.alldatasheet.com/datasheet-pdf/view/1303687/WINSEN/MH-Z19C.html

## More infos about the MHZ sensors an wiring:
 - https://revspace.nl/MHZ19
 - https://wolles-elektronikkiste.de/en/mh-z14-and-mh-z19-co2-sensors
 - https://emariete.com/en/sensor-co2-mh-z19b/


## Further reading:
- [Building a MH-Z19 CO2 meter](http://ihormelnyk.com/Page/mh-z19_co2_meter)

(russuian, but google translate does a good job)
- https://geektimes.ru/post/285572/
- https://geektimes.ru/post/278178/
