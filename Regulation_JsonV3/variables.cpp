#include "config.h"

// Seuils
float highThreshold = 26.0;
float lowThreshold = 24.0;
int fireThreshold = 100;
float fireTemperatureThreshold = 23.5;

// Variables globales
float currentTemperature = 0.0;
int currentLight = 0;

bool heaterState = false;
bool coolerState = false;
int fanSpeed = 0;
bool fireDetected = false;
bool fireBlockingFan = false;

bool manualOverride = false;
unsigned long overrideStartTime = 0;

String target_ip = "";
int target_port = 1880;
int target_sp = 2;

// Moyennage
const int SAMPLES_COUNT = 30;
float tempSamples[SAMPLES_COUNT];
int lightSamples[SAMPLES_COUNT];
int sampleIndex = 0;
float lastAvgTemperature = 0;
int lastAvgLight = 0;

// Objets
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Adafruit_NeoPixel strip(NUM_LEDS, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);
AsyncWebServer server(80);
WiFiMulti wm;

// MQTT Configuration
const char* mqtt_broker = "172.20.10.3"; // ETABLIR LA VALEUR d'IP
const int mqtt_port = 1883;
const char* mqtt_topic = "uca/iot/master";

// Piscine (déclarations uniques)
bool isOccupied = false;
bool isHotspot = false;
int lightOccupancyThreshold = 500;

const int MAX_POOLS = 10;
PoolData otherPools[MAX_POOLS];
int poolCount = 0;

WiFiClient espClient;
PubSubClient mqttClient(espClient);