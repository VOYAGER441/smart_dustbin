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
  uint16_t _wet_ultrasonic_value = 0;
  uint16_t _dry_ultrasonic_value = 0;
  uint8_t _servo_angle = 0;

  void _setWetPart();
  void _setDryPart();
  bool _isDustbinFull();
  bool _isAnyDustbinFull(uint16_t dry_level, uint16_t wet_level) const;
  void _init_wifi();
  bool _send_trash_level_to_backend(uint16_t dry_level, uint16_t wet_level);

public:
  void init();
  void moisture_cal();
  uint16_t check_trash_level_for_dry();
  uint16_t check_trash_level_for_wet();
  void check_and_report_trash_levels();
};

} // namespace esp32s3_ns
