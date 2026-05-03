#pragma once
#include <Arduino.h>
#include <Wire.h>

#include "./utils/config.h"
#include "motor/Servo_m.h"
#include "sensors/Soil_Moisture.h"
#include "sensors/Ultrasonic.h"

namespace esp32s3_ns {
class ESP32_S3 {
private:
  servo_ns::Servo _servo;
  soil_ns::Soil_Moisture _soilSensor;
  ultrasonic_ns::Ultrasonic _ultrasonicSensor;

  uint8_t _soil_moisture_value = 0;
  uint8_t _wet_ultrasonic_value = 0;
  uint8_t _dry_ultrasonic_value = 0;
  uint8_t _servo_angle = 0;

  void _setWetPart();
  void _setDryPart();
  bool _isDustbinFull();

public:
  void init();
  void moisture_cal();
  uint8_t check_trash_level_for_dry();
  uint8_t check_trash_level_for_wet();
};

} // namespace esp32s3_ns