#include "ESP32-S3.h"

void esp32s3_ns::ESP32_S3::init() {

  // init motors
  esp32s3_ns::ESP32_S3::_servo.init(); // Initialize the servo motor
}

// ==============================================================
// public function
// ==============================================================
// calculate the moisture value and open the dustbin accordingly, if the dustbin
// is full, then do nothing
void esp32s3_ns::ESP32_S3::moisture_cal() {

  // ckeck the dustbin is empty or not by using ultrasonic sensor
  if (_isDustbinFull()) {
    return; // If the dustbin is full, exit the function
  }

  _soil_moisture_value = esp32s3_ns::ESP32_S3::_soilSensor
                             .get_moisture(); // Get moisture percentage

  if (_soil_moisture_value < 40) {
    _setDryPart();
  } else if (_soil_moisture_value >= 40) {
    _setWetPart();

  } else {
    _setDryPart();
  }
}

// check the trash level for dry dustbin, return the distance value in cm
uint8_t esp32s3_ns::ESP32_S3::check_trash_level_for_dry(){
  _dry_ultrasonic_value = esp32s3_ns::ESP32_S3::_ultrasonicSensor.get_distance_in_cm(dry_TRIG_PIN, dry_ECHO_PIN);
  
  return _dry_ultrasonic_value;
}

// check the trash level for wet dustbin, return the distance value in cm
uint8_t esp32s3_ns::ESP32_S3::check_trash_level_for_wet(){
  _wet_ultrasonic_value = esp32s3_ns::ESP32_S3::_ultrasonicSensor.get_distance_in_cm(wet_TRIG_PIN, wet_ECHO_PIN);
  
  return _wet_ultrasonic_value;
}

// ==============================================================
// private function
// ==============================================================

// check if the dustbin is full by using ultrasonic sensor, if the distance is
// less than 25cm, then it is full
bool esp32s3_ns::ESP32_S3::_isDustbinFull() {

  uint8_t temp_ultrasonic_value_for_dry =
      esp32s3_ns::ESP32_S3::_ultrasonicSensor.get_distance_in_cm(dry_TRIG_PIN,
                                                                 dry_ECHO_PIN);

  uint8_t temp_ultrasonic_value_for_wet =
      esp32s3_ns::ESP32_S3::_ultrasonicSensor.get_distance_in_cm(wet_TRIG_PIN,
                                                                 wet_ECHO_PIN);

  // if the dustbin is full, then send a notification to the user and play the
  // buzzer

  // Check if dry dustbin is full
  if (temp_ultrasonic_value_for_dry <= DUSTBIN_FULL_THRESHOLD) {
    Serial.println("Dry dustbin is full! Please empty it.");
    // Play a tone on the buzzer for dry bin
    tone(BUZZER_PIN, BUZZER_FREQUENCY_FOR_DRY, BUZZER_DURATION);
    return true;
  }

  // Check if wet dustbin is full
  if (temp_ultrasonic_value_for_wet <= DUSTBIN_FULL_THRESHOLD) {
    Serial.println("Wet dustbin is full! Please empty it.");
    // Play a tone on the buzzer for wet bin
    tone(BUZZER_PIN, BUZZER_FREQUENCY_FOR_WET, BUZZER_DURATION);
    return true;
  }

  // Both dustbins are not full
  Serial.println("Both dustbins are not full yet.");
  return false;
}

// open the dustbin right part to let the wet dust fall into the bin
void esp32s3_ns::ESP32_S3::_setWetPart() {

  // wet part open the dustbin
  _servo.set_angle(180); // Open the dustbin right part

  delay(5000); // Wait for 5 seconds

  // back to close the dustbin
  _servo.set_angle(90); // Close the dustbin
}

// open the dustbin left part to let the dry dust fall into the bin
void esp32s3_ns::ESP32_S3::_setDryPart() {
  // dry part, open the dustbin
  _servo.set_angle(0); // Open the dustbin left part

  // wait for 5 seconds to let the dustbin open and the dust fall into the bin
  delay(5000); // Wait for 5 seconds

  // back to close the dustbin
  _servo.set_angle(90); // Close the dustbin
}
