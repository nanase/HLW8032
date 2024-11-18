# HLW8032

## About

This library decodes serial data sent by the HLW8032 and makes it available as various readings.

The library is built without relying on a specific serial communication implementation, taking into account its various platform variants, and was inspired by Ahmed ARIF's implementation of [the Hlw8032 library](https://github.com/ahmedarif193/Hlw8032).

> HLW8032 is a high-precision electric energy metering IC which is manufactured using CMOS process and is mainly applied to single-phase application. It is able to measure line voltage and current and calculate active power, apparent power and power factor. (from HLW8032 User manual REV 1.5)

## Example

This example is an Arduino sketch that will be run on Arduino.
See [the examples directory](./examples/) for a complete example.

```c++
#include <Arduino.h>
#include "hlw8032.h"

HLW8032 sensor;

#define PIN_IN_SD 39

void setup() {
  Serial.begin(115200);
  Serial1.begin(4800, SERIAL_8E1, PIN_IN_SD);
}

void loop() {
  while (Serial1.available()) {
    sensor.processData(Serial1.read());
  }

  Serial.printf(">EffectiveVoltage:%f\n", sensor.getEffectiveVoltage());
  delay(1000);
}
```

## Notice: Poor accuracy at low power

Several designers have reported not being able to get correct current and power values from the HLW8032. Although not stated in the datasheet, the HLW8032 may measure incorrect values at low power. The accuracy is very low, especially at no load. Therefore, the values calculated from the current values are also affected, such as apparent power, power factor, and electrical quantity. Active power also takes time to get correct readings at low loads.

To work around these problems, _always_ use a dummy load of about 10W or use a different IC for measurements.

Since the board on which the HLW8032 is used varies from designer to designer, we are unable to respond to requests that the values cannot be get correctly. We apologize for any inconvenience.

## License

[**LGPLv3**](./LICENSE)

As mentioned above, this library is based on [Ahmed ARIF's library](https://github.com/ahmedarif193/Hlw8032). Therefore, the license of his library applies to this license as well.
