#include "hlw8032.h"

#ifndef bitRead
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#endif

/**
 * Initialize a new HLW8032 class instance by specifying
 *  the resistance value of the implemented resistor.
 *
 * @param voltageR1 Resistance of the first resistor for voltage division for
 *                  voltage measurement. Unit is ohm.
 * @param voltageR2 Resistance of the second resistor for voltage division for
 *                  voltage measurement. Unit is ohm.
 * @param currentShuntR Shunt resistance value for current measurement.
 *                      Unit is ohm.
 */
HLW8032::HLW8032(float voltageR1, float voltageR2, float currentShuntR) {
  voltageCoefficient = voltageR1 / (1000.0f * voltageR2);
  currentCoefficient = 1.0f / (1000.0f * currentShuntR);

  reset();
}

/**
 * Processes a byte of data received from the HLW8032 and attempts to decode it.
 *
 * @param data 1 byte of serial data received from HLW8032.
 * @return True if decoding is complete with all necessary data, false otherwise.
 */
bool HLW8032::processData(uint8_t data) {
  if (data == 0x55) {
    headerFound = true;
    buffer[0]   = data;
  } else if (data == 0x5a && headerFound) {
    buffer[1] = data;
    index     = 2;
    check     = 0;
  } else if (index >= 2 && index < 24) {
    buffer[index] = data;

    if (index < 23) {
      check += data;
    }

    index++;

    if (index == 24) {
      if (check == buffer[23]) {
        processFrame();
      }

      index       = 0;
      headerFound = false;

      return true;
    }
  } else {
    index       = 0;
    headerFound = false;
  }

  return false;
}

/**
 * Discard the observed data and initialize the state.
 */
void HLW8032::reset() {
  index       = 0;
  headerFound = false;
  check       = 0;

  voltageParameter = 0;
  voltage          = 0;
  currentParameter = 0;
  current          = 0;
  powerParameter   = 0;
  power            = 0;
  pf               = 0;
  pfData           = 0;
}

void HLW8032::processFrame() {
  voltageParameter = (buffer[2] << 16) + (buffer[3] << 8) + buffer[4];
  currentParameter = (buffer[8] << 16) + (buffer[9] << 8) + buffer[10];
  powerParameter   = (buffer[14] << 16) + (buffer[15] << 8) + buffer[16];

  if (bitRead(buffer[20], 6)) {
    voltage = (buffer[5] << 16) + (buffer[6] << 8) + buffer[7];
  }

  if (bitRead(buffer[20], 5)) {
    current = (buffer[11] << 16) + (buffer[12] << 8) + buffer[13];
  }

  if (bitRead(buffer[20], 4)) {
    power = (buffer[17] << 16) + (buffer[18] << 8) + buffer[19];
  }

  pf = (buffer[21] << 8) + buffer[22];

  if (bitRead(buffer[20], 7)) {
    pfData++;
  }
}

/**
 * Check the integrity of the received data.
 *
 * @return True if the result of the checksum calculation is valid, false otherwise.
 */
bool HLW8032::passedChecksum() {
  uint8_t check = 0;

  for (uint8_t i = 2; i <= 22; i++) {
    check += buffer[i];
  }

  return check == buffer[23];
}

/**
 * Get the measured RMS voltage. The unit is V.
 *
 * @return The measured RMS voltage.
 */
float HLW8032::getEffectiveVoltage() {
  return ((float)voltageParameter / (float)voltage) * voltageCoefficient;
}

/**
 * Get the measured RMS current. The unit is A.
 *
 * @return The measured RMS current.
 */
float HLW8032::getEffectiveCurrent() {
  return ((float)currentParameter / (float)current) * currentCoefficient;
}

/**
 * Get the measured active power. The unit is W.
 *
 * @return The measured active power.
 */
float HLW8032::getActivePower() {
  return ((float)powerParameter / (float)power) * voltageCoefficient * currentCoefficient;
}

/**
 * Get the measured apparent power. The unit is W.
 *
 * @return The measured apparent power.
 */
float HLW8032::getApparentPower() {
  return getEffectiveVoltage() * getEffectiveCurrent();
}

/**
 * Get the measured power factor.
 *
 * @return The measured power factor.
 */
float HLW8032::getPowerFactor() {
  return getActivePower() / getApparentPower();
}

/**
 * Get the measured electrical quantity. The unit is kWh.
 *
 * @return The measured electrical quantity.
 */
float HLW8032::getElectricalQuantity() {
  // According to the data sheet, the following formula is correct,
  // but the formula is transformed to ensure accuracy.
  // float pfCount = (1.0f / powerParameter) * (1.0f / apparentPower) * 10e9 * 3600;
  float pulseCount = 10e9 * 3600 / powerParameter / getApparentPower();

  return (pfData * 65536 + pf) / pulseCount;
}
