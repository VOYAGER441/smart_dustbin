#include "Soil_Moisture.h";

uint8_t soil_ns::Soil_Moisture::get_moisture() {
  soil_ns::Soil_Moisture::_moisture_value = analogRead(MOISTURE_PIN);
  
  soil_ns::Soil_Moisture::_moisturePercentage =
      map(soil_ns::Soil_Moisture::_moisture_value, MOISTURE_FROM_LOW,
          MOISTURE_FROM_HIGH, MOISTURE_TO_LOW, MOISTURE_TO_HIGH);
  
          return soil_ns::Soil_Moisture::_moisturePercentage;
}