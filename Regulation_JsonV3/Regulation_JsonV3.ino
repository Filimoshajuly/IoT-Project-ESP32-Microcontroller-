#include "config.h"


void setup() {
    Serial.begin(115200);
    delay(2000);
    
    setup_wifi();
    WiFi.setSleep(true);                    // WiFi economie

esp_wifi_set_max_tx_power(20);          // réduire la puissance

btStop(); //etteindre bluetooth

 setCpuFrequencyMhz(80);  //reduire la fréquence

 Serial.println("Mode économie d'énergie activé");
 
    mqttClient.setServer(mqtt_broker, mqtt_port);
    mqttClient.setBufferSize(1024);
    mqttClient.setCallback(mqttCallback);


    if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
        Serial.println("Erreur: impossible de monter LittleFS");
    } else {
        Serial.println("LittleFS monté avec succès");
        loadThresholds();
    }

    sensors.begin();
    
    pinMode(ONBOARD_LED, OUTPUT);
    digitalWrite(ONBOARD_LED, LOW);
    pinMode(COLD_LED, OUTPUT);
    pinMode(HEAT_LED, OUTPUT);
    digitalWrite(COLD_LED, LOW);
    digitalWrite(HEAT_LED, LOW);
    
    strip.begin();
    strip.setBrightness(BRIGHTNESS);
    strip.show();
    
    ledcAttach(FAN_PWM, FAN_FREQ, FAN_RES);
    ledcWrite(FAN_PWM, 0);
    
    for (int i = 0; i < SAMPLES_COUNT; i++) {
        tempSamples[i] = 0;
        lightSamples[i] = 0;
    }
    
    setupWebServer();
    server.begin();
    Serial.println("Server started on port 80");
    Serial.println("Setup terminé");
}
// IMPORTANT - mqtt est active. IL FAUT etablir l'adresse IP CORRECT dans variables.cpp const char* mqtt_broker (ligne 42!). Sinon il faut commenter mqtt ici
void loop() {
    unsigned long currentMillis = millis();
     mqttClient.loop(); 
     if (!mqttClient.connected()) {
        reconnectMQTT();
    }
   // Allumer LED - HOTSPOT
    if (isHotspot) {
        digitalWrite(ONBOARD_LED, HIGH);
    } else {
        digitalWrite(ONBOARD_LED, LOW);
    }
    

    receiveThresholds();
    handleSensorsAndActuators(currentMillis);
    handleFireDetection(currentMillis);
    handleJsonSending(currentMillis);

    delay(10);
}