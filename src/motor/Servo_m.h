#pragma once
#include "../utils/config.h"
#include <Arduino.h>
#include <Servo.h>
namespace servo_ns {
extern ::Servo myServo; // Defined once in Servo_m.cpp
class Servo {
private:
  /* data */
  ::Servo _servo;
  uint8_t _currentAngle = 0;

public:
  void init();

  void set_angle(uint8_t angle);
  uint8_t get_angle() const { return _currentAngle; }
};

} // namespace servo_ns