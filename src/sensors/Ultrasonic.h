#pragma once
#include "../utils/config.h"
#include <Arduino.h>

namespace ultrasonic_ns {

class Ultrasonic {
private:
  uint16_t _read_once_in_cm(uint8_t trig_pin, uint8_t echo_pin) const;
public:
  // returns distance in cm
  uint16_t get_distance_in_cm(uint8_t trig_pin, uint8_t echo_pin) const;
};
} // namespace ultrasonic_ns
