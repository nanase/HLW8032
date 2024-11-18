#include <Arduino.h>
#include <M5AtomS3.h>

#include "hlw8032.h"

HLW8032 sensor;

// Specify the port number corresponding to the GPIO of the board.
// For example : G39->39
#define PIN_IN_SD 39

void setup() {
  Serial.begin(115200);
  Serial1.begin(4800, SERIAL_8E1, PIN_IN_SD);
}

void loop() {
  bool sensorUpdated = false;

  while (Serial1.available()) {
    sensorUpdated |= sensor.processData(Serial1.read());
  }

  if (sensorUpdated) {
    // Teleplot format
    Serial.printf(">EffectiveVoltage:%f\n", sensor.getEffectiveVoltage());
    Serial.printf(">EffectiveCurrent:%f\n", sensor.getEffectiveCurrent());
    Serial.printf(">ActivePower:%f\n", sensor.getActivePower());
    Serial.printf(">ApparentPower:%f\n", sensor.getApparentPower());
    Serial.printf(">PowerFactor:%f\n", sensor.getPowerFactor());
    Serial.printf(">ElectricalQuantity:%f\n", sensor.getElectricalQuantity());
  }

  delay(1000);
}
