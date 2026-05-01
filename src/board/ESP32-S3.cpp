#include "ESP32-S3.h"

void esp32s3_ns::ESP32_S3::init() {

  // init motors
  esp32s3_ns::ESP32_S3::_servo.init(); // Initialize the servo motor
}

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

// ==============================================================
// private function
// ==============================================================

// check if the dustbin is full by using ultrasonic sensor, if the distance is less than 25cm, then it is full
bool esp32s3_ns::ESP32_S3::_isDustbinFull() {
  uint8_t temp_ultrasonic_value =
      esp32s3_ns::ESP32_S3::_ultrasonicSensor.get_distance_in_cm();

  // if the dustbin is full, then send a notification to the user and play the
  // buzzer
  if (temp_ultrasonic_value <= DUSTBIN_FULL_THRESHOLD) {
    Serial.println("Dustbin is full! Please empty it.");
    // Play a tone on the buzzer
    tone(BUZZER_PIN, BUZZER_FREQUENCY, BUZZER_DURATION); // Play a tone at 1000 Hz
    return true;
  } else {
    Serial.println("Dustbin is not full yet.");
    return false;
  }
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
