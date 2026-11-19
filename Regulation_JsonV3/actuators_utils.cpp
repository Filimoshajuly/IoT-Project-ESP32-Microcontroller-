#include "config.h"

void updateSimpleLeds(float temperature) {
    if (manualOverride && (millis() - overrideStartTime < 5000)) {
        return;
    }
    manualOverride = false;
    
    if (temperature > highThreshold) {
        coolerState = true;
        heaterState = false;
        digitalWrite(COLD_LED, HIGH);
        digitalWrite(HEAT_LED, LOW);
    } else if (temperature < lowThreshold) {
        coolerState = false;
        heaterState = true;
        digitalWrite(COLD_LED, LOW);
        digitalWrite(HEAT_LED, HIGH);
    } else {
        coolerState = false;
        heaterState = false;
        digitalWrite(COLD_LED, LOW);
        digitalWrite(HEAT_LED, LOW);
    }
}

void updateLedStrip(float temperature) {
    uint32_t color;
    if (temperature > highThreshold) {
        color = strip.Color(255, 0, 0);
    } else if (temperature < lowThreshold) {
        color = strip.Color(0, 0, 255);
    } else {
        color = strip.Color(0, 255, 0);
    }
    for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, color);
    }
    strip.show();
}

void updateFan(float temperature) {
    if (manualOverride && (millis() - overrideStartTime < 5000)) {
        return;
    }
    manualOverride = false;
    
    if (fireBlockingFan) {
        if (fanSpeed != 0) {
            fanSpeed = 0;
            ledcWrite(FAN_PWM, 0);
        }
        return;
    }
    
    if (temperature > highThreshold) {
        float diff = temperature - highThreshold;
        fanSpeed = (int)(diff * 50);
        fanSpeed = constrain(fanSpeed, 0, 255);
        if (fanSpeed < 50 && fanSpeed > 0) fanSpeed = 50;
    } else {
        fanSpeed = 0;
    }
    ledcWrite(FAN_PWM, fanSpeed);
}