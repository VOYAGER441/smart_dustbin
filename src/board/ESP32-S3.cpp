#include "ESP32-S3.h"
#include <HTTPClient.h>
#include <WiFi.h>

void esp32s3_ns::ESP32_S3::init() {
  pinMode(wet_TRIG_PIN, OUTPUT);
  pinMode(wet_ECHO_PIN, INPUT);
  pinMode(dry_TRIG_PIN, OUTPUT);
  pinMode(dry_ECHO_PIN, INPUT);
  pinMode(soil_TRIG_PIN, OUTPUT);
  pinMode(soil_ECHO_PIN, INPUT);
  pinMode(MOISTURE_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(wet_TRIG_PIN, LOW);
  digitalWrite(dry_TRIG_PIN, LOW);
  digitalWrite(soil_TRIG_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  analogReadResolution(MOISTURE_ADC_RESOLUTION_BITS);

  // init motors
  esp32s3_ns::ESP32_S3::_servo.init(); // Initialize the servo motor
  _init_wifi();
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
uint16_t esp32s3_ns::ESP32_S3::check_trash_level_for_dry() {
  _dry_ultrasonic_value =
      esp32s3_ns::ESP32_S3::_ultrasonicSensor.get_distance_in_cm(dry_TRIG_PIN,
                                                                 dry_ECHO_PIN);

  return _dry_ultrasonic_value;
}

// check the trash level for wet dustbin, return the distance value in cm
uint16_t esp32s3_ns::ESP32_S3::check_trash_level_for_wet() {
  _wet_ultrasonic_value =
      esp32s3_ns::ESP32_S3::_ultrasonicSensor.get_distance_in_cm(wet_TRIG_PIN,
                                                                 wet_ECHO_PIN);

  return _wet_ultrasonic_value;
}

void esp32s3_ns::ESP32_S3::check_and_report_trash_levels() {
  const uint16_t dry_level = check_trash_level_for_dry();
  const uint16_t wet_level = check_trash_level_for_wet();

  Serial.print("Dry level (cm): ");
  Serial.println(dry_level);
  Serial.print("Wet level (cm): ");
  Serial.println(wet_level);

  if (!_isAnyDustbinFull(dry_level, wet_level)) {
    Serial.println("Dustbin levels are below full threshold.");
    return;
  }

  Serial.println("At least one dustbin is full. Sending levels to backend.");
  _send_trash_level_to_backend(dry_level, wet_level);
}

uint16_t esp32s3_ns::ESP32_S3::check_ultrasonic_distance(uint8_t trig_pin,
                                                        uint8_t echo_pin) {
  return esp32s3_ns::ESP32_S3::_ultrasonicSensor.get_distance_in_cm(trig_pin,
                                                                 echo_pin);
}

// ==============================================================
// private function
// ==============================================================

// check if the dustbin is full by using ultrasonic sensor, if the distance is
// less than 25cm, then it is full
bool esp32s3_ns::ESP32_S3::_isDustbinFull() {
  const uint16_t temp_ultrasonic_value_for_dry = check_trash_level_for_dry();
  const uint16_t temp_ultrasonic_value_for_wet = check_trash_level_for_wet();

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

bool esp32s3_ns::ESP32_S3::_isAnyDustbinFull(uint16_t dry_level,
                                             uint16_t wet_level) const {
  return (dry_level <= DUSTBIN_FULL_THRESHOLD) ||
         (wet_level <= DUSTBIN_FULL_THRESHOLD);
}

void esp32s3_ns::ESP32_S3::_init_wifi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");

  uint8_t attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi connected. IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connection failed.");
  }
}

bool esp32s3_ns::ESP32_S3::_send_trash_level_to_backend(uint16_t dry_level,
                                                         uint16_t wet_level) {
  if (WiFi.status() != WL_CONNECTED) {
    _init_wifi();
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Skipping API call because WiFi is not connected.");
    return false;
  }

  HTTPClient http;
  http.setTimeout(API_TIMEOUT_MS);
  http.begin(BACKEND_API_URL);
  http.addHeader("Content-Type", "application/json");

  const String payload =
      "{\"dry_level\":" + String(dry_level) + ",\"wet_level\":" + String(wet_level) + "}";

  const int http_code = http.POST(payload);
  if (http_code > 0) {
    Serial.print("Backend response code: ");
    Serial.println(http_code);
    Serial.print("Backend response body: ");
    Serial.println(http.getString());
  } else {
    Serial.print("HTTP request failed: ");
    Serial.println(http.errorToString(http_code));
  }

  http.end();
  return http_code >= 200 && http_code < 300;
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
