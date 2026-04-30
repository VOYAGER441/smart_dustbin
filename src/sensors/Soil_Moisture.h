#pragma once
#include "../utils/config.h"
#include <Arduino.h>
namespace soil_ns {
class Soil_Moisture {
private:
  /* data */
  uint16_t _moisture_value = 0;
  uint8_t _moisturePercentage = 0;

public:
  uint8_t get_moisture();
};

} // namespace soil_ns