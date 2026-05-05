#include "Soil_Moisture.h"

uint8_t soil_ns::Soil_Moisture::get_moisture() {
  soil_ns::Soil_Moisture::_moisture_value = analogRead(MOISTURE_PIN);

  const long mapped_moisture =
      map(soil_ns::Soil_Moisture::_moisture_value, MOISTURE_FROM_LOW,
          MOISTURE_FROM_HIGH, MOISTURE_TO_LOW, MOISTURE_TO_HIGH);
  soil_ns::Soil_Moisture::_moisturePercentage =
      static_cast<uint8_t>(constrain(mapped_moisture, 0L, 100L));

  return soil_ns::Soil_Moisture::_moisturePercentage;
}
