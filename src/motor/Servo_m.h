// #pragma once
// #include "../utils/config.h"
// #include <Arduino.h>
// #if defined(ARDUINO_ARCH_ESP32)
// #if __has_include(<ESP32Servo.h>)
// #include <ESP32Servo.h>
// #else
// #error "ESP32Servo library is required for ESP32 builds."
// #endif
// #else
// #include <Servo.h>
// #endif
// namespace servo_ns {
// extern ::Servo myServo; // Defined once in Servo_m.cpp
// class Servo {
// private:
//   /* data */
//   ::Servo _servo;
//   uint8_t _currentAngle = 0;

// public:
//   void init();

//   void set_angle(uint8_t angle);
//   uint8_t get_angle() const { return _currentAngle; }
// };

// } // namespace servo_ns


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