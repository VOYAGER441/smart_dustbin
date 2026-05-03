#include "Ultrasonic.h"


uint16_t ultrasonic_ns::Ultrasonic::get_distance_in_cm(uint8_t trig_pin, uint8_t echo_pin) {
    
    digitalWrite(trig_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_pin, LOW);

    unsigned long duration = pulseIn(echo_pin, HIGH, 30000);
    if (duration == 0) return 0;

    uint16_t distance_cm = static_cast<uint16_t>(duration / 58UL);
    
    return distance_cm;
}

