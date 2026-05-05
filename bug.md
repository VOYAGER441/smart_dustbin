# Bug Report

## 1. Moisture routing runs every loop (object-detection gate is bypassed)
- **File:** `src/main.cpp` (lines 21-26)
- **Bug:** `esp32s3.moisture_cal()` is called inside the soil-distance condition **and again unconditionally** right after it.
- **Impact:** The lid-routing logic runs even when no object is near the sensor, causing unnecessary servo movement and incorrect behavior.
- **Fix direction:** Remove the unconditional call and keep only the gated call.

## 2. Sensor pins are never configured in the active `src/` firmware
- **Files:** `src/main.cpp`, `src/board/ESP32-S3.cpp` (no `pinMode(...)` calls)
- **Bug:** Ultrasonic trigger/echo, moisture input, and buzzer pins are used without explicit pin mode setup.
- **Impact:** Unreliable readings and output behavior on real hardware (especially trigger pins that should be `OUTPUT`).
- **Fix direction:** Initialize all hardware pins during startup (for example in `ESP32_S3::init()`).

## 3. Ultrasonic timeout is interpreted as “bin full”
- **File:** `src/sensors/Ultrasonic.cpp` (lines 10-11)
- **Bug:** On timeout (`duration == 0`), `get_distance_in_cm()` returns `0`.
- **Impact:** Full-bin checks treat `0 cm` as very close (`<= DUSTBIN_FULL_THRESHOLD`), producing false “bin full” detections.
- **Fix direction:** Return a large fallback distance on timeout (for example `400`) or handle timeout as an invalid reading.

## 4. Compiler warning from invalid include directive
- **File:** `src/sensors/Soil_Moisture.cpp` (line 1)
- **Bug:** `#include "Soil_Moisture.h";` has an extra `;`.
- **Impact:** Reproducible compiler warning (`extra tokens at end of #include directive`), which can hide more important warnings in CI/logs.
- **Fix direction:** Change to `#include "Soil_Moisture.h"`.

## 5. Moisture percentage conversion can be wrong on ESP32 ADC range
- **File:** `src/sensors/Soil_Moisture.cpp` + `src/utils/config.h`
- **Bug:** Mapping assumes raw range `0..1023`, but ESP32 ADC commonly returns up to `4095` unless resolution is changed.
- **Impact:** Reported moisture percentage may be incorrect or overflow when converted to `uint8_t`.
- **Fix direction:** Use the actual ADC resolution (or set it explicitly) and constrain the mapped result to `0..100` before casting.


# ALL BUG FIXED 