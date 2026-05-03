#include "./motor/Servo_m.h"
#include "board/ESP32-S3.h"

esp32s3_ns::ESP32_S3 esp32s3;
unsigned long lastTrashCheckTime = 0;

void setup() {
  Serial.begin(115200);
  esp32s3.init();
}

void loop() {
  const unsigned long now = millis();
  if (now - lastTrashCheckTime >= TRASH_CHECK_INTERVAL) {
    lastTrashCheckTime = now;
    esp32s3.check_and_report_trash_levels();
  }

  esp32s3.moisture_cal();
  delay(1000);
}

