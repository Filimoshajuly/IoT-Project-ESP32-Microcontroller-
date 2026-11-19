#include "config.h"

void addSample(float temp, int light) {
    tempSamples[sampleIndex] = temp;
    lightSamples[sampleIndex] = light;
    sampleIndex++;
    if (sampleIndex >= SAMPLES_COUNT) {
        sampleIndex = 0;
    }
}

void calculateAverages(float &avgTemp, int &avgLight) {
    float sumTemp = 0;
    long sumLight = 0;
    for (int i = 0; i < SAMPLES_COUNT; i++) {
        sumTemp += tempSamples[i];
        sumLight += lightSamples[i];
    }
    avgTemp = sumTemp / SAMPLES_COUNT;
    avgLight = sumLight / SAMPLES_COUNT;
}

void checkFireDetection(float avgTemperature, int avgLight) {
    bool fireByLight = (avgLight > fireThreshold);
    bool fireByTemp = (avgTemperature > fireTemperatureThreshold);
    bool fire = fireByLight && fireByTemp;
    
    if (fire) {
        if (!fireDetected) {
            fireDetected = true;
            fireBlockingFan = true;
            Serial.println("!!! ALARME INCENDIE DÉTECTÉE !!!");
            fanSpeed = 0;
            ledcWrite(FAN_PWM, 0);
        }
        digitalWrite(ONBOARD_LED, HIGH);
    } else {
        if (fireDetected) {
            fireDetected = false;
            fireBlockingFan = false;
            Serial.println("--- Fin de l'alarme incendie ---");
        }
        digitalWrite(ONBOARD_LED, LOW);
    }
}

void handleFireDetection(unsigned long currentMillis) {
    static unsigned long lastFireCheck = 0;
    if (currentMillis - lastFireCheck >= 3000) {
        lastFireCheck = currentMillis;
        float avgTemp;
        int avgLight;
        calculateAverages(avgTemp, avgLight);
        checkFireDetection(avgTemp, avgLight);
    }
}