#include <Arduino.h>
#include <unity.h>

#include "hlw8032.h"

const float voltageCoefficient = 1.88;
const float currentCoefficient = 1.00;

HLW8032 sensor;

uint8_t correctData[] = {
  0x55,              // State register
  0x5a,              // Check register
  0x02, 0xeb, 0xa8,  // Voltage parameter register
  0x00, 0x0d, 0xac,  // Voltage register
  0x00, 0x3e, 0xe4,  // Current parameter register
  0x00, 0xdb, 0x2b,  // Current register
  0x51, 0x11, 0xe8,  // Power parameter register
  0x19, 0xbd, 0x29,  // Power register
  0x71,              // Data update register
  0x00, 0xed,        // PF register
  0x1d               // CheckSum register
};

void setUp() {
  sensor.reset();
}

void testReset() {
  // Initially, all registers are set to zero and the various calculated values
  // are NaN, not infinite.
  TEST_ASSERT_FLOAT_IS_NAN(sensor.getEffectiveVoltage());
  TEST_ASSERT_FLOAT_IS_NAN(sensor.getEffectiveCurrent());
  TEST_ASSERT_FLOAT_IS_NAN(sensor.getActivePower());
  TEST_ASSERT_FLOAT_IS_NAN(sensor.getApparentPower());
  TEST_ASSERT_FLOAT_IS_NAN(sensor.getPowerFactor());
  TEST_ASSERT_FLOAT_IS_NAN(sensor.getElectricalQuantity());
}

void testProcessData() {
  // Even with valid serial data, the processData function will always return
  // false if not all 24 bytes are processed.
  for (size_t i = 0; i < 23; i++) {
    TEST_ASSERT_FALSE(sensor.processData(correctData[i]));
  }

  // When all 24 bytes of valid data have been processed,
  // the processData function returns true.
  TEST_ASSERT_TRUE(sensor.processData(correctData[23]));
}

void testGetValues() {
  for (uint8_t data : correctData) {
    sensor.processData(data);
  }

  TEST_ASSERT_TRUE(sensor.passedChecksum());
  TEST_ASSERT_FLOAT_WITHIN(0.001, 102.80914285714285, sensor.getEffectiveVoltage());
  TEST_ASSERT_FLOAT_WITHIN(0.001, 0.2869517172545315, sensor.getEffectiveCurrent());
  TEST_ASSERT_FLOAT_WITHIN(0.001, 5.921444133208839, sensor.getActivePower());
  TEST_ASSERT_FLOAT_WITHIN(0.001, 29.50126009232359, sensor.getApparentPower());
  TEST_ASSERT_FLOAT_WITHIN(0.001, 0.20071834608683833, sensor.getPowerFactor());
  TEST_ASSERT_FLOAT_WITHIN(0.001, 0.0010318729495642255, sensor.getElectricalQuantity());
}

void setup() {
  delay(3000);

  UNITY_BEGIN();
  RUN_TEST(testReset);
  RUN_TEST(testProcessData);
  RUN_TEST(testGetValues);
  UNITY_END();
}

void loop() {}
