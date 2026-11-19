#include "config.h"

void setup_wifi() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    delay(100);
    
    wm.addAP("iPhone (Iuliia)", "12345bbb");
    wm.addAP("Numericable-2853", "cvnsw7dvccxf");
    wm.addAP("IOT", "iotmiage");
    
    Serial.print("Connexion WiFi...");
    while (wm.run() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnecté ! IP: " + WiFi.localIP().toString());
}

void wifi_printstatus() {
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n--- WiFi Status ---");
        Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str()); 
        Serial.printf("SSID: %s\n", WiFi.SSID().c_str()); 
        Serial.printf("Signal Strength (RSSI): %ld dBm\n", WiFi.RSSI()); 
        Serial.println("-------------------\n");
    }
}