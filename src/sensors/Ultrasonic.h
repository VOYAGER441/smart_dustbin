#pragma once
#include "../utils/config.h"
#include <Arduino.h>

namespace ultrasonic_ns {

class Ultrasonic {
private:
  /* data */
public:
  //returns distance in cm
  static uint16_t get_distance();
};
} // namespace ultrasonic_ns