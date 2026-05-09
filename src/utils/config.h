// ultrasonic sensor configuration
#define wet_TRIG_PIN 14      // D5
#define wet_ECHO_PIN 12      // D6 - add voltage divider!
#define dry_TRIG_PIN 13      // D7
#define dry_ECHO_PIN 5       // D1 - add voltage divider!
#define soil_TRIG_PIN 4      // D2
#define soil_ECHO_PIN 15     // D8 - add voltage divider!

// soil moisture sensor configuration
#define MOISTURE_PIN A0
#define MOISTURE_ADC_RESOLUTION_BITS 10   // ⚠️ ESP8266 is 10-bit NOT 12-bit
#define MOISTURE_FROM_LOW 0
#define MOISTURE_FROM_HIGH ((1 << MOISTURE_ADC_RESOLUTION_BITS) - 1)  // = 1023
#define MOISTURE_TO_LOW 0
#define MOISTURE_TO_HIGH 100

// servo motor configuration
#define SERVO_PIN 0          // D3 ⚠️ disconnect during flashing

// buzzer configuration
#define BUZZER_PIN 2        // D4
#define BUZZER_FREQUENCY_FOR_DRY 1000
#define BUZZER_FREQUENCY_FOR_WET 2000
#define BUZZER_DURATION 2000

// dustbin full distance threshold in cm
#define DUSTBIN_FULL_THRESHOLD 25
#define MOISTURE_CHECK_DISTANCE_THRESHOLD 10
#define ULTRASONIC_TIMEOUT_DISTANCE_CM 400

// trash monitoring + backend API configuration
#define TRASH_CHECK_INTERVAL 6000UL
#define WIFI_SSID "mr.robot"
#define WIFI_PASSWORD "kernalpanic"
#define BACKEND_API_URL "http://192.168.1.8:5000/api/v1/health"
#define API_TIMEOUT_MS 5000