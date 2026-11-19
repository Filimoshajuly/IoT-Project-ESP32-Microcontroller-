#include "config.h"


void receiveThresholds() {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        if (input.length() > 0 && input.startsWith("{") && input.endsWith("}")) {
            StaticJsonDocument<256> doc;
            deserializeJson(doc, input);
            bool updated = false;
            if (doc["regul"]["ht"]) {
                highThreshold = doc["regul"]["ht"];
                updated = true;
            }
            if (doc["regul"]["lt"]) {
                lowThreshold = doc["regul"]["lt"];
                updated = true;
            }
            if (updated) saveThresholds();
        }
    }
}

void calculateHotspot() {
    // Par défaut, pas hotspot
    isHotspot = false;
    
    // Si on n'a pas encore reçu de données d'autres piscines
    if (poolCount == 0) {
        return;
    }
    
    // Trouver la température maximale parmi toutes les piscines
    float maxTemp = currentTemperature;
    for (int i = 0; i < poolCount; i++) {
        if (otherPools[i].temperature > maxTemp) {
            maxTemp = otherPools[i].temperature;
        }
    }
    
    // Si ma température est la plus élevée (ou égale) et > 25°C
    if (currentTemperature >= maxTemp && currentTemperature > 25.0) {
        isHotspot = true;
    }
}

void handleJsonSending(unsigned long currentMillis) {
    static unsigned long lastSendTime = 0;
    if (target_sp <= 0) return;
    
    if (currentMillis - lastSendTime >= (target_sp * 1000)) {
        lastSendTime = currentMillis;
        calculateHotspot();
        isOccupied = (currentLight > lightOccupancyThreshold);
        StaticJsonDocument<1024> doc;
        
        // ========== STATUS ==========
        doc["status"]["temperature"] = currentTemperature;
        doc["status"]["light"] = currentLight;
        doc["status"]["regul"] = (heaterState || coolerState) ? "RUNNING" : "HALT";
        doc["status"]["fire"] = fireDetected;
        doc["status"]["heat"] = heaterState ? "ON" : "OFF";
        doc["status"]["cold"] = coolerState ? "ON" : "OFF";
        doc["status"]["fanspeed"] = fanSpeed;
        
        // ========== LOCATION ==========
        doc["location"]["room"] = "312";
        doc["location"]["address"] = "Les lucioles";  
        doc["location"]["gps"]["lat"] = 43.6953;   
        doc["location"]["gps"]["lon"] = 7.1296;      
        
        // ========== REGUL ==========
        doc["regul"]["lt"] = lowThreshold;
        doc["regul"]["ht"] = highThreshold;
        
        // ========== INFO ==========
        doc["info"]["ident"] = "P_22511057_ЮЛИЯ И МАЭЛЛА";
        doc["info"]["user"] = "ЮЛИЯ И МАЭЛЛА";                
        doc["info"]["loc"] = "Biot";                        
        
        // ========== NET ==========
        doc["net"]["uptime"] = String(millis() / 1000);
        doc["net"]["ssid"] = WiFi.SSID();
        doc["net"]["mac"] = WiFi.macAddress();
        doc["net"]["ip"] = WiFi.localIP().toString();
        
        // ========== REPORTHOST ==========
        doc["reporthost"]["target_ip"] = target_ip;         
        doc["reporthost"]["target_port"] = target_port;     
        doc["reporthost"]["sp"] = target_sp;     

        // ========== PISCINE ==========
        doc["piscine"]["occupied"] = isOccupied;    // boolean
        doc["piscine"]["hotspot"] = isHotspot;              
        
        String jsonPayload;
        serializeJson(doc, jsonPayload);
        Serial.println("" + jsonPayload);
        // Envoi HTTP
        if (WiFi.status() == WL_CONNECTED && target_ip != "") {
            HTTPClient http;
            String url = "http://" + target_ip + ":" + String(target_port) + "/esp?mac=" + WiFi.macAddress();
            http.begin(url);
            http.addHeader("Content-Type", "application/json");
            int httpResponseCode = http.POST(jsonPayload);
            if (httpResponseCode > 0) {
                Serial.printf("", httpResponseCode);
            } else {
                Serial.printf(" ", http.errorToString(httpResponseCode).c_str());
            }
            http.end();
        }
       // Envoi MQTT
if (mqttClient.connected()) {
    // publish main topic
    bool published1 = mqttClient.publish(mqtt_topic, (const uint8_t*)jsonPayload.c_str(), jsonPayload.length(), true);
    
    // Publish topic carte (uca/iot/piscine)
    bool published2 = mqttClient.publish("uca/iot/piscine", jsonPayload.c_str());
    
    if (published1 || published2) {
        //Serial.println("[MQTT] Publication réussie");
    } else {
        Serial.println("[MQTT] Échec de publication");
    }
} else {
    Serial.println("[MQTT] Client non connecté, tentative de reconnexion...");
    reconnectMQTT();
}

}
}