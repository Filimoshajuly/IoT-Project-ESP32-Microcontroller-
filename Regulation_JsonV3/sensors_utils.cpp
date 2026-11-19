#include "config.h"

float readTemperature() {
    sensors.requestTemperatures();
    float temp = sensors.getTempCByIndex(0);
    if (temp == DEVICE_DISCONNECTED_C) {
        Serial.println("ERREUR: DS18B20 non connecté !");
        return -127.0;
    }
    return temp;
}

int readLight() {
    return 4096 - analogRead(LIGHT_PIN);
}