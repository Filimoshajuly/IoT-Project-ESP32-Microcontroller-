#include "config.h"

void handleSensorsAndActuators(unsigned long currentMillis) {
    static unsigned long lastSampleTime = 0;
    if (currentMillis - lastSampleTime >= 100) {
        lastSampleTime = currentMillis;
        currentTemperature = readTemperature();
        currentLight = readLight();
        if (currentTemperature == DEVICE_DISCONNECTED_C) {
            currentTemperature = -127.0;
            Serial.println("ERREUR: DS18B20 non connecté");
        }
        addSample(currentTemperature, currentLight);
        if (currentTemperature > -100) {
            updateSimpleLeds(currentTemperature);
            updateLedStrip(currentTemperature);
            updateFan(currentTemperature);
        }
    }
}