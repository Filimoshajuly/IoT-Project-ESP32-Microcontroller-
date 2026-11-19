#include "config.h"

String processor(const String &var) {
    if (var == "TEMPERATURE") return String(currentTemperature);
    if (var == "LIGHT") return String(currentLight);
    if (var == "HEATER") return heaterState ? "ON" : "OFF";
    if (var == "COOLER") return coolerState ? "ON" : "OFF";
    if (var == "FIRE") return String(fireDetected);
    if (var == "LT") return String(lowThreshold);
    if (var == "HT") return String(highThreshold);
    if (var == "WHERE") return "Les lucioles, 06560 Valbonne";
    if (var == "SSID") return WiFi.SSID();
    if (var == "MAC") return WiFi.macAddress();
    if (var == "IP") return WiFi.localIP().toString();
    if (var == "UPTIME") return String(millis() / 1000);
    if (var == "PRT_IP") return target_ip.isEmpty() ? "127.0.0.1" : target_ip;
    if (var == "PRT_PORT") return String(target_port);
    if (var == "PRT_T") return String(target_sp);
    return String();
}

void setupWebServer() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/index.html", String(), false, processor);
    });
    
    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
        StaticJsonDocument<512> jsonDoc;
        jsonDoc["temperature"] = currentTemperature;
        jsonDoc["light"] = currentLight;
        jsonDoc["heat"] = heaterState ? "ON" : "OFF";
        jsonDoc["cold"] = coolerState ? "ON" : "OFF";
        jsonDoc["fire"] = fireDetected;
        jsonDoc["uptime"] = millis() / 1000;
        jsonDoc["mac"] = WiFi.macAddress();
        jsonDoc["ip"] = WiFi.localIP().toString();
        String response;
        serializeJson(jsonDoc, response);
        request->send(200, "application/json", response);
    });
    
    server.on("/value", HTTP_GET, [](AsyncWebServerRequest *request){
        StaticJsonDocument<256> jsonDoc;
        bool hasValidArg = false;
        if (request->hasArg("temperature")) { jsonDoc["temperature"] = currentTemperature; hasValidArg = true; }
        if (request->hasArg("light")) { jsonDoc["light"] = currentLight; hasValidArg = true; }
        if (request->hasArg("fire")) { jsonDoc["fire"] = fireDetected; hasValidArg = true; }
        if (request->hasArg("ht")) { jsonDoc["ht"] = highThreshold; hasValidArg = true; }
        if (request->hasArg("lt")) { jsonDoc["lt"] = lowThreshold; hasValidArg = true; }
        if (!hasValidArg) { request->send(404, "text/plain", "Not Found"); return; }
        String response;
        serializeJson(jsonDoc, response);
        request->send(200, "application/json", response);
    });
    
    server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request){
        bool modified = false;
        if (request->hasArg("ht")) { highThreshold = request->arg("ht").toFloat(); modified = true; }
        if (request->hasArg("lt")) { lowThreshold = request->arg("lt").toFloat(); modified = true; }
        if (request->hasArg("cool")) {
            String val = request->arg("cool");
            if (val == "on") { coolerState = true; digitalWrite(COLD_LED, HIGH); manualOverride = true; overrideStartTime = millis(); }
            else if (val == "off") { coolerState = false; digitalWrite(COLD_LED, LOW); manualOverride = true; overrideStartTime = millis(); }
            modified = true;
        }
        if (request->hasArg("heat")) {
            String val = request->arg("heat");
            if (val == "on") { heaterState = true; digitalWrite(HEAT_LED, HIGH); manualOverride = true; overrideStartTime = millis(); }
            else if (val == "off") { heaterState = false; digitalWrite(HEAT_LED, LOW); manualOverride = true; overrideStartTime = millis(); }
            modified = true;
        }
        if (request->hasArg("fan")) {
            String val = request->arg("fan");
            if (val == "on") { fanSpeed = 255; ledcWrite(FAN_PWM, fanSpeed); manualOverride = true; overrideStartTime = millis(); }
            else if (val == "off") { fanSpeed = 0; ledcWrite(FAN_PWM, fanSpeed); manualOverride = true; overrideStartTime = millis(); }
            modified = true;
        }
        if (modified) { saveThresholds(); request->send(200, "text/plain", "Seuils mis a jour !"); }
        else { request->send(404, "text/plain", "Argument non reconnu"); }
    });
    
    server.on("/target", HTTP_POST, [](AsyncWebServerRequest *request){
        //Serial.println("Requête POST /target reçue !");
        if (request->hasArg("ip") && request->hasArg("port") && request->hasArg("sp")) {
            target_ip = request->arg("ip");
            target_port = request->arg("port").toInt();
            target_sp = request->arg("sp").toInt();
            request->send(200, "text/plain", "Reporting configure avec succes !");
        } else {
            request->send(400, "text/plain", "Erreur : Parametres manquants");
        }
    });
    
    server.on("/esp.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/esp.css", "text/css");
    });
}