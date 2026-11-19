#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArduinoOTA.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <PubSubClient.h>
#include "esp_wifi.h"
#include "esp_bt.h"

// ============================================================
// PINS CAPTEURS
// ============================================================
#define ONE_WIRE_BUS 23
#define LIGHT_PIN A5

// ============================================================
// PINS ACTIONNEURS
// ============================================================
#define COLD_LED 19
#define HEAT_LED 21
#define FAN_PWM 27
#define LED_STRIP_PIN 13
#define ONBOARD_LED 2

// ============================================================
// PARAMÈTRES LED STRIP
// ============================================================
#define NUM_LEDS 5
#define BRIGHTNESS 50

// ============================================================
// PARAMÈTRES VENTILATEUR
// ============================================================
#define FAN_FREQ 25000
#define FAN_RES 8

// ============================================================
// CONSTANTES LITTLEFS
// ============================================================
#define FORMAT_LITTLEFS_IF_FAILED true

// ============================================================
// VARIABLES GLOBALES (déclarations extern)
// ============================================================
extern float highThreshold;
extern float lowThreshold;
extern int fireThreshold;
extern float fireTemperatureThreshold;

extern float currentTemperature;
extern int currentLight;
extern bool heaterState;
extern bool coolerState;
extern int fanSpeed;
extern bool fireDetected;
extern bool fireBlockingFan;
extern bool manualOverride;
extern unsigned long overrideStartTime;

extern String target_ip;
extern int target_port;
extern int target_sp;

// Moyennage
extern const int SAMPLES_COUNT;
extern float tempSamples[];
extern int lightSamples[];
extern int sampleIndex;
extern float lastAvgTemperature;
extern int lastAvgLight;

extern bool isOccupied;
extern  bool isHotspot;

// Objets
extern OneWire oneWire;
extern DallasTemperature sensors;
extern Adafruit_NeoPixel strip;
extern AsyncWebServer server;
extern WiFiMulti wm;

// ============================================================
// MQTT CONFIGURATION
// ============================================================
extern const char* mqtt_broker;
extern const int mqtt_port;
extern const char* mqtt_topic;
extern WiFiClient espClient;
extern PubSubClient mqttClient;

// ============================================================
// PROTOTYPES DES FONCTIONS
// ============================================================
void setup_wifi();
void saveThresholds();
void loadThresholds();
void setupWebServer();
void receiveThresholds();
void handleSensorsAndActuators(unsigned long currentMillis);
void handleFireDetection(unsigned long currentMillis);
void handleJsonSending(unsigned long currentMillis);
String processor(const String &var);
float readTemperature();
int readLight();
void updateSimpleLeds(float temperature);
void updateLedStrip(float temperature);
void updateFan(float temperature);
void addSample(float temp, int light);
void calculateAverages(float &avgTemp, int &avgLight);
void checkFireDetection(float avgTemperature, int avgLight);
void wifi_printstatus();


// ============================================================
// PROTOTYPES DES FONCTIONS MQTT
// ============================================================
void reconnectMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);

// ============================================================
// STRUCTURES POUR LES DONNÉES DES AUTRES PISCINES
// ============================================================
struct PoolData {
    String ident;
    float temperature;
    float lat;
    float lon;
};

extern const int MAX_POOLS;
extern PoolData otherPools[];
extern int poolCount;
extern int lightOccupancyThreshold;

// ============================================================
// PROTOTYPES DES FONCTIONS
// ============================================================
void calculateHotspot();  

#endif