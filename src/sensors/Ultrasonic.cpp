#include "Ultrasonic.h"

uint16_t ultrasonic_ns::Ultrasonic::_read_once_in_cm(uint8_t trig_pin,
                                                     uint8_t echo_pin) const {
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);

  const unsigned long duration = pulseIn(echo_pin, HIGH, 30000UL);
  if (duration == 0) {
    return ULTRASONIC_TIMEOUT_DISTANCE_CM;
  }

  return static_cast<uint16_t>(duration / 58UL);
}

uint16_t ultrasonic_ns::Ultrasonic::get_distance_in_cm(uint8_t trig_pin,
                                                       uint8_t echo_pin) const {
  uint16_t valid_samples[5];
  uint8_t valid_count = 0;

  for (uint8_t i = 0; i < 5; i++) {
    const uint16_t reading = _read_once_in_cm(trig_pin, echo_pin);
    if (reading != ULTRASONIC_TIMEOUT_DISTANCE_CM) {
      valid_samples[valid_count++] = reading;
    }
    delay(50);
  }

  if (valid_count == 0) {
    return ULTRASONIC_TIMEOUT_DISTANCE_CM;
  }

  for (uint8_t i = 1; i < valid_count; i++) {
    uint8_t j = i;
    while (j > 0 && valid_samples[j - 1] > valid_samples[j]) {
      const uint16_t tmp = valid_samples[j - 1];
      valid_samples[j - 1] = valid_samples[j];
      valid_samples[j] = tmp;
      j--;
    }
  }

  return valid_samples[valid_count / 2];
}
