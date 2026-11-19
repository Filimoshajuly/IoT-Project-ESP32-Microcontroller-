#include "config.h"

// ============================================================
// CONNEXION AU BROKER MQTT
// ============================================================
void reconnectMQTT() {
    static uint8_t attemptCount = 0;
    
    while (!mqttClient.connected()) {
        String clientId = "ESP32_";
        clientId += WiFi.macAddress();
        
        Serial.print("Tentative de connexion MQTT (");
        Serial.print(++attemptCount);
        Serial.print(")...");
        
        if (mqttClient.connect(clientId.c_str())) {
            
            attemptCount = 0;
            
            // SUBSCRIBE
            mqttClient.subscribe("uca/iot/piscine");
            Serial.println("Abonné à uca/iot/piscine");
            
        } else {
            Serial.print(" Échec, état=");
            Serial.println(mqttClient.state());
            delay(5000);
        }
    }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String message;
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error) return;
    
    // Ignorer nos propres messages
    String ident = doc["info"]["ident"] | "";
    if (ident == "P_22511057_ЮЛИЯ И МАЭЛЛА") return;
    
    float temp = doc["status"]["temperature"] | -127.0;
    float lat = doc["location"]["gps"]["lat"] | 0;
    float lon = doc["location"]["gps"]["lon"] | 0;
    
    // Mettre à jour ou ajouter la piscine
    for (int i = 0; i < poolCount; i++) {
        if (otherPools[i].ident == ident) {
            otherPools[i].temperature = temp;
            otherPools[i].lat = lat;
            otherPools[i].lon = lon;
            return;
        }
    }
    
    // Ajouter nouvelle piscine si la place
    if (poolCount < MAX_POOLS) {
        otherPools[poolCount] = {ident, temp, lat, lon};
        poolCount++;
        Serial.print("Nouvelle piscine détectée: ");
        Serial.println(ident);
    }
}