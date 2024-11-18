#pragma once

#include "stddef.h"
#include "stdint.h"

/**
 * Decode the serial data received from the HLW8032 to obtain readings.
 */
class HLW8032 {
 public:
  HLW8032(float voltageR1 = 1880e3, float voltageR2 = 1e3, float currentShuntR = 0.001);

  bool processData(uint8_t data);

  void reset();

  bool passedChecksum();
  float getEffectiveVoltage();
  float getEffectiveCurrent();
  float getActivePower();
  float getApparentPower();
  float getPowerFactor();
  float getElectricalQuantity();

 private:
  uint8_t buffer[25];
  size_t index;
  bool headerFound;
  uint8_t check;

  float voltageCoefficient;
  float currentCoefficient;

  uint32_t voltageParameter;
  uint32_t voltage;
  uint32_t currentParameter;
  uint32_t current;
  uint32_t powerParameter;
  uint32_t power;
  uint32_t pf;
  uint32_t pfData;

  void processFrame();
};
