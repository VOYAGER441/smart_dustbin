#include "Servo_m.h"

void servo_ns::Servo::init() {
  _servo.attach(SERVO_PIN);
  set_angle(90);
}

void servo_ns::Servo::set_angle(uint8_t angle) {
  _currentAngle = constrain(angle, 0, 180); // Arduino built-in, clean & safe
  _servo.write(_currentAngle);
}