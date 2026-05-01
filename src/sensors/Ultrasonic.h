#pragma once
#include "../utils/config.h"
#include <Arduino.h>

namespace ultrasonic_ns {

class Ultrasonic {
private:
  /* data */
public:
  //returns distance in cm
   uint16_t get_distance_in_cm();
};
} // namespace ultrasonic_ns