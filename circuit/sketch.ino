#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>

#if defined(ARDUINO_ARCH_ESP32)
class ServoDriver {
private:
  uint8_t _pin = 255;
  bool _attached = false;
  static constexpr uint16_t SERVO_FRAME_US = 20000;

public:
  bool attach(uint8_t pin) {
    _pin = pin;
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    _attached = true;
    return true;
  }

  void write(uint8_t angle) {
    if (!_attached) {
      return;
    }

    angle = static_cast<uint8_t>(constrain(angle, 0, 180));
    const uint16_t pulseWidthUs =
        static_cast<uint16_t>(map(angle, 0, 180, 500, 2400));
    const uint16_t restUs = SERVO_FRAME_US - pulseWidthUs;

    // Send a short burst of servo pulses to move and hold position.
    for (uint8_t i = 0; i < 25; i++) {
      digitalWrite(_pin, HIGH);
      delayMicroseconds(pulseWidthUs);
      digitalWrite(_pin, LOW);
      delayMicroseconds(restUs);
    }
  }

  void detach() {
    _attached = false;
    if (_pin != 255) {
      digitalWrite(_pin, LOW);
    }
  }
};
#else
#include <Servo.h>
using ServoDriver = Servo;
#endif

// Ultrasonic sensor pins
constexpr uint8_t WET_TRIG_PIN = 6;
constexpr uint8_t WET_ECHO_PIN = 7;
constexpr uint8_t DRY_TRIG_PIN = 4;
constexpr uint8_t DRY_ECHO_PIN = 5;
constexpr uint8_t SOIL_TRIG_PIN = 17;
constexpr uint8_t SOIL_ECHO_PIN = 18;

// Soil moisture sensor pin and mapping
constexpr uint8_t MOISTURE_PIN = 16;
constexpr int MOISTURE_FROM_LOW = 0;
constexpr int MOISTURE_FROM_HIGH = 1023;
constexpr int MOISTURE_TO_LOW = 0;
constexpr int MOISTURE_TO_HIGH = 100;
constexpr uint8_t MOISTURE_DRY_THRESHOLD = 40;
constexpr uint16_t MOISTURE_CHECK_DISTANCE_THRESHOLD = 10;

// Servo + buzzer pins
constexpr uint8_t SERVO_PIN = 15;
constexpr uint8_t BUZZER_PIN = 1;

// Dustbin full threshold in cm
constexpr uint16_t DUSTBIN_FULL_THRESHOLD = 25;

// Buzzer behavior
constexpr uint16_t BUZZER_FREQUENCY_FOR_DRY = 1000;
constexpr uint16_t BUZZER_FREQUENCY_FOR_WET = 2000;
constexpr uint16_t BUZZER_DURATION_MS = 2000;
constexpr unsigned long BUZZER_ALERT_COOLDOWN_MS = 5000UL;

// Monitoring + backend settings
constexpr unsigned long TRASH_CHECK_INTERVAL_MS = 60000UL; // 1 min
constexpr uint16_t API_TIMEOUT_MS = 5000;

const char *WIFI_SSID = "Mainak_777";
const char *WIFI_PASSWORD = "1234567890";
const char *BACKEND_API_URL = "http://your-backend-url/api/trash-level";

ServoDriver lidServo;

unsigned long lastTrashCheckMs = 0;
unsigned long lastBuzzerAlertMs = 0;

uint16_t dryLevelCm = 0;
uint16_t wetLevelCm = 0;

uint16_t readDistanceCmOnce(uint8_t trigPin, uint8_t echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  const unsigned long duration = pulseIn(echoPin, HIGH, 30000UL);
  if (duration == 0) {
    return 400; // timeout: treat as "far", not full
  }

  return static_cast<uint16_t>(duration / 58UL);
}

uint16_t readDistanceCmFiltered(uint8_t trigPin, uint8_t echoPin) {
  uint16_t samples[3];
  for (uint8_t i = 0; i < 3; i++) {
    samples[i] = readDistanceCmOnce(trigPin, echoPin);
    delay(20);
  }

  if (samples[0] > samples[1]) {
    const uint16_t tmp = samples[0];
    samples[0] = samples[1];
    samples[1] = tmp;
  }
  if (samples[1] > samples[2]) {
    const uint16_t tmp = samples[1];
    samples[1] = samples[2];
    samples[2] = tmp;
  }
  if (samples[0] > samples[1]) {
    const uint16_t tmp = samples[0];
    samples[0] = samples[1];
    samples[1] = tmp;
  }

  return samples[1]; // median
}

void refreshTrashLevels() {
  dryLevelCm = readDistanceCmFiltered(DRY_TRIG_PIN, DRY_ECHO_PIN);
  wetLevelCm = readDistanceCmFiltered(WET_TRIG_PIN, WET_ECHO_PIN);
}

bool isAnyDustbinFull(uint16_t dryLevel, uint16_t wetLevel) {
  return (dryLevel <= DUSTBIN_FULL_THRESHOLD) || (wetLevel <= DUSTBIN_FULL_THRESHOLD);
}

void alertIfDustbinFull(uint16_t dryLevel, uint16_t wetLevel) {
  if (!isAnyDustbinFull(dryLevel, wetLevel)) {
    return;
  }

  const unsigned long now = millis();
  if (now - lastBuzzerAlertMs < BUZZER_ALERT_COOLDOWN_MS) {
    return;
  }
  lastBuzzerAlertMs = now;

  if (dryLevel <= DUSTBIN_FULL_THRESHOLD) {
    Serial.println("Dry dustbin is full! Please empty it.");
    tone(BUZZER_PIN, BUZZER_FREQUENCY_FOR_DRY, BUZZER_DURATION_MS);
    return;
  }

  Serial.println("Wet dustbin is full! Please empty it.");
  tone(BUZZER_PIN, BUZZER_FREQUENCY_FOR_WET, BUZZER_DURATION_MS);
}

bool connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
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
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection failed.");
    return false;
  }

  Serial.print("WiFi connected. IP: ");
  Serial.println(WiFi.localIP());
  return true;
}

bool sendTrashLevelsToBackend(uint16_t dryLevel, uint16_t wetLevel) {
  if (!connectWiFi()) {
    Serial.println("Skipping API call because WiFi is not connected.");
    return false;
  }

  HTTPClient http;
  http.setTimeout(API_TIMEOUT_MS);
  http.begin(BACKEND_API_URL);
  http.addHeader("Content-Type", "application/json");

  const String payload = "{\"dry_level\":" + String(dryLevel) +
                         ",\"wet_level\":" + String(wetLevel) + "}";

  const int httpCode = http.POST(payload);
  if (httpCode > 0) {
    Serial.print("Backend response code: ");
    Serial.println(httpCode);
    Serial.print("Backend response body: ");
    Serial.println(http.getString());
  } else {
    Serial.print("HTTP request failed: ");
    Serial.println(http.errorToString(httpCode));
  }

  http.end();
  return httpCode >= 200 && httpCode < 300;
}

void checkAndReportTrashLevels() {
  refreshTrashLevels();

  Serial.print("Dry level (cm): ");
  Serial.println(dryLevelCm);
  Serial.print("Wet level (cm): ");
  Serial.println(wetLevelCm);

  if (!isAnyDustbinFull(dryLevelCm, wetLevelCm)) {
    Serial.println("Dustbin levels are below full threshold.");
    return;
  }

  Serial.println("At least one dustbin is full. Sending levels to backend.");
  sendTrashLevelsToBackend(dryLevelCm, wetLevelCm);
}

uint8_t readMoisturePercent() {
  const int raw = analogRead(MOISTURE_PIN);
  const long mapped = map(raw, MOISTURE_FROM_LOW, MOISTURE_FROM_HIGH,
                          MOISTURE_TO_LOW, MOISTURE_TO_HIGH);
  return static_cast<uint8_t>(constrain(mapped, 0, 100));
}

void openDryPart() {
  lidServo.write(0);
  delay(5000);
  lidServo.write(90);
}

void openWetPart() {
  lidServo.write(180);
  delay(5000);
  lidServo.write(90);
}

void routeTrashByMoisture() {
  refreshTrashLevels();
  alertIfDustbinFull(dryLevelCm, wetLevelCm);
  if (isAnyDustbinFull(dryLevelCm, wetLevelCm)) {
    return;
  }

  const uint16_t soilDistanceCm =
      readDistanceCmFiltered(SOIL_TRIG_PIN, SOIL_ECHO_PIN);
  if (soilDistanceCm >= MOISTURE_CHECK_DISTANCE_THRESHOLD) {
    return;
  }

  const uint8_t moisturePercent = readMoisturePercent();
  Serial.print("Moisture (%): ");
  Serial.println(moisturePercent);

  if (moisturePercent < MOISTURE_DRY_THRESHOLD) {
    openDryPart();
  } else {
    openWetPart();
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(DRY_TRIG_PIN, OUTPUT);
  pinMode(DRY_ECHO_PIN, INPUT);
  pinMode(WET_TRIG_PIN, OUTPUT);
  pinMode(WET_ECHO_PIN, INPUT);
  pinMode(SOIL_TRIG_PIN, OUTPUT);
  pinMode(SOIL_ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MOISTURE_PIN, INPUT);

  lidServo.attach(SERVO_PIN);
  lidServo.write(90);

  connectWiFi();
}

void loop() {
  const unsigned long now = millis();
  if (now - lastTrashCheckMs >= TRASH_CHECK_INTERVAL_MS) {
    lastTrashCheckMs = now;
    checkAndReportTrashLevels();
  }

  routeTrashByMoisture();
  delay(1000);
}
