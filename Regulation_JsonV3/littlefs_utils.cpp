#include "config.h"

void saveThresholds() {
    File file = LittleFS.open("/config.json", FILE_WRITE);
    if (!file) {
        Serial.println("Erreur: impossible d'ouvrir le fichier pour écriture");
        return;
    }
    file.print("{\"lt\":");
    file.print(lowThreshold);
    file.print(",\"ht\":");
    file.print(highThreshold);
    file.println("}");
    file.close();
    Serial.println("Seuils sauvegardés dans LittleFS");
}

void loadThresholds() {
    if (!LittleFS.exists("/config.json")) {
        Serial.println("Fichier config.json non trouvé, utilisation des seuils par défaut");
        saveThresholds();
        return;
    }
    File file = LittleFS.open("/config.json", "r");
    if (!file) {
        Serial.println("Erreur: impossible d'ouvrir le fichier pour lecture");
        return;
    }
    String content = file.readString();
    file.close();
    StaticJsonDocument<200> doc;
    deserializeJson(doc, content);
    if (doc["lt"]) lowThreshold = doc["lt"];
    if (doc["ht"]) highThreshold = doc["ht"];
    Serial.print("Seuils chargés: bas = ");
    Serial.print(lowThreshold);
    Serial.print(", haut = ");
    Serial.println(highThreshold);
}