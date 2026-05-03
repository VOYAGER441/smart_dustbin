#include "./motor/Servo_m.h"
#include "board/ESP32-S3.h"

esp32s3_ns::ESP32_S3 esp32s3;
void setup() {
  esp32s3.init();
}

void loop() {
  // check the trash level of both dry and wet dustbin every 5 seconds, if the dustbin is full, then send a notification to backend 
  esp32s3.check_trash_level_for_dry();
  esp32s3.check_trash_level_for_wet();
  delay(5000); // Wait for 5 seconds
}


