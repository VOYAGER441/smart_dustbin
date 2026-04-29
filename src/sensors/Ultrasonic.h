#pragma once
#include "config.h"
#include <stdint.h>

namespace ultrasonic_ns {

class Ultrasonic {
private:
  /* data */
public:
  static uint16_t get_distance();
};
} // namespace ultrasonic_ns