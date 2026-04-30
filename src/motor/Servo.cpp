#include "Servo.h"

void servo_ns::Servo::init() {
  // Initialize the servo motor here
  // For example, you can attach the servo to a specific pin
  // and set its initial position if needed.
  myServo.attach(SERVO_PIN); // Replace SERVO_PIN with the actual pin number
  myServo.write(0);          // Set initial position to 0 degrees
}

void servo_ns::Servo::set_angle(uint8_t angle) {
  // Set the servo angle here
  // Ensure that the angle is within the valid range (0-180 degrees)
  if (angle > 180) {
    angle = 180; // Limit to maximum angle
  } else if (angle < 0) {
    angle = 0; // Limit to minimum angle
  } else {
    angle = angle; // Use the provided angle
  }
  
  myServo.write(angle); // Set the servo to the specified angle
}