#include "Ultrasonic.h"


uint16_t ultrasonic_ns::Ultrasonic::get_distance(){
    
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000);
    if (duration == 0) return 0;

    uint16_t distance_cm = static_cast<uint16_t>(duration / 58UL);
    
    return distance_cm;
}

