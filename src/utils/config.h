// ultrasonic sencor configuration
#define wet_TRIG_PIN 16
#define wet_ECHO_PIN 17
#define dry_TRIG_PIN 4
#define dry_ECHO_PIN 5


// soil moisture sensor configuration
#define MOISTURE_PIN 15
#define MOISTURE_FROM_LOW 0
#define MOISTURE_FROM_HIGH 1023
#define MOISTURE_TO_LOW 0
#define MOISTURE_TO_HIGH 100

// servo motor configuration
#define SERVO_PIN 18


// buzzer configuration
#define BUZZER_PIN 19
#define BUZZER_FREQUENCY_FOR_DRY 1000
#define BUZZER_FREQUENCY_FOR_WET 2000
#define BUZZER_DURATION 2000

// dustbin full distance threshold in cm
#define DUSTBIN_FULL_THRESHOLD 25

// trash monitoring + backend API configuration
#define TRASH_CHECK_INTERVAL 60000UL
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define BACKEND_API_URL "http://your-backend-url/api/trash-level"
#define API_TIMEOUT_MS 5000
