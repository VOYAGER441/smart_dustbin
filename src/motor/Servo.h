#pragma once
#include "../utils/config.h"
#include <Arduino.h>
#include <Servo.h>
namespace servo_ns {
Servo myServo; // Create a Servo object
class Servo {
private:
  /* data */
public:
  void init();

  void set_angle(uint8_t angle);
};

} // namespace servo_ns