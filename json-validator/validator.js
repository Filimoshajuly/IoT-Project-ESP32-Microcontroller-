// ============================================
// VALIDATEUR JSON POUR ESP32
// ============================================

const Ajv = require('ajv');
const ajv = new Ajv({ allErrors: true });

// SCHEMA (règles à respecter pour le JSON)
const schema = {
  type: "object",
  required: ["status", "location", "regul", "info", "net", "reporthost", "piscine"],
  properties: {
    "status": {
      type: "object",
      required: ["temperature", "light", "regul", "fire", "heat", "cold", "fanspeed"],
      properties: {
        "temperature": { type: "number" },
        "light": { type: "integer" },
        "regul": { type: "string", enum: ["RUNNING", "HALT"] },
        "fire": { type: "boolean" },
        "heat": { type: "string", enum: ["ON", "OFF"] },
        "cold": { type: "string", enum: ["ON", "OFF"] },
        "fanspeed": { type: "integer", minimum: 0, maximum: 255 }
      }
    },
    "location": {
      type: "object",
      required: ["room", "gps", "address"],
      properties: {
        "room": { type: "string" },
        "gps": {
          type: "object",
          required: ["lat", "lon"],
          properties: {
            "lat": { type: "number" },
            "lon": { type: "number" }
          }
        },
        "address": { type: "string" }
      }
    },
    "regul": {
      type: "object",
      required: ["lt", "ht"],
      properties: {
        "lt": { type: "number" },
        "ht": { type: "number" }
      }
    },
    "info": {
      type: "object",
      required: ["ident", "user", "loc"],
      properties: {
        "ident": { type: "string" },
        "user": { type: "string" },
        "loc": { type: "string" }
      }
    },
    "net": {
      type: "object",
      required: ["uptime", "ssid", "mac", "ip"],
      properties: {
        "uptime": { type: "string" },
        "ssid": { type: "string" },
        "mac": { type: "string" },
        "ip": { type: "string" }
      }
    },
    "reporthost": {
      type: "object",
      required: ["target_ip", "target_port", "sp"],
      properties: {
        "target_ip": { type: "string" },
        "target_port": { type: "integer" },
        "sp": { type: "integer" }
      }
    },
    "piscine": {
      type: "object",
      required: ["occupied", "hotspot"],
      properties: {
        "occupied": { type: "boolean" },
        "hotspot": { type: "boolean" }
      }
    }
  }
};

// JSON CORRECT (doit passer la validation - Benchmark)
const correctJson = `{
  "status": {
    "temperature": 24.5,
    "light": 1872,
    "regul": "HALT",
    "fire": false,
    "heat": "OFF",
    "cold": "OFF",
    "fanspeed": 0
  },
  "location": {
    "room": "312",
    "gps": { "lat": 43.62453842, "lon": 7.050628185 },
    "address": "Les lucioles"
  },
  "regul": { "lt": 24, "ht": 26 },
  "info": { "ident": "ESP32_001", "user": "Student", "loc": "Biot" },
  "net": { "uptime": "45", "ssid": "N/A", "mac": "N/A", "ip": "N/A" },
  "reporthost": { "target_ip": "127.0.0.1", "target_port": 1880, "sp": 2 },
  "piscine": { "occupied": false, "hotspot": false }
}`;

// JSON À TESTER (votre JSON actuel)
const myJson = `{"status":{"temperature":25.5,"light":2256,"regul":"RUNNING","fire":false,"heat":"OFF","cold":"ON","fanspeed":50},"location":{"room":"312","gps":{"lat":43.62453842,"lon":7.050628185},"address":"Les lucioles"},"regul":{"lt":24,"ht":25.3},"info":{"ident":"ESP32_001","user":"Votre_Nom","loc":"Biot"},"net":{"uptime":"289","ssid":"iPhone (Iuliia)","mac":"0C:DC:7E:2C:17:88","ip":"172.20.10.8"},"reporthost":{"target_ip":"","target_port":1880,"sp":2},"piscine":{"occupied":true,"hotspot":true}}`;

// FONCTION DE VALIDATION
function validateJson(jsonString, testName) {
  console.log("\n" + "═".repeat(60));
  console.log(`📋 TEST: ${testName}`);
  console.log("═".repeat(60));
  
  let jsonData;
  try {
    jsonData = JSON.parse(jsonString);
    console.log("✅ SYNTAXE: OK");
  } catch (error) {
    console.log("❌ SYNTAXE: ERREUR!");
    console.log(`   ${error.message}`);
    return false;
  }

  const isValid = ajv.validate(schema, jsonData);
  
  if (isValid) {
    console.log("✅ SCHEMA: OK");
    console.log("🎉 RESULTAT: JSON VALIDE!");
    return true;
  } else {
    console.log("❌ SCHEMA: ERREURS!");
    ajv.errors.forEach((err, index) => {
      console.log(`   ${index + 1}. Champ "${err.instancePath}" - ${err.message}`);
    });
    console.log("🎉 RESULTAT: JSON INVALIDE!");
    return false;
  }
}

console.log("\n");
console.log("═══════════════════════════════════════════");
console.log("   VALIDATEUR JSON POUR ESP32");
console.log("═══════════════════════════════════════════");

validateJson(correctJson, "TEST 1: JSON CORRECT (Benchmark)");
validateJson(myJson, "TEST 2: JSON À TESTER");

console.log("\n Prêt! Pour tester votre JSON, modifiez la variable myJson\n");