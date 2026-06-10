/**
 * Dragon Capsule Telemetry System
 * GS 2026 - FIAP - Edge Computing & Computer Systems
 * Integração: ESP32 -> MQTT -> FIWARE (IoT Agent UL)
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ===== Rede WiFi (Wokwi) =====
const char* WIFI_SSID     = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

// ===== FIWARE / MQTT =====
const char* MQTT_BROKER = "35.199.64.218"; 
const int   MQTT_PORT   = 1883;

// Padrão FIWARE IoT Agent
const char* TOPIC_PUB = "/dragon2026/dragon001/attrs";

// ===== Pinos =====
#define PIN_DHT       4
#define PIN_POT_PRES  34
#define PIN_POT_CO2   35
#define PIN_POT_VEL   32
#define PIN_LED_OK    2
#define PIN_LED_ALERT 5
#define PIN_BUZZER    18

// ===== Limites de Alerta =====
#define TEMP_MAX  30.0
#define TEMP_MIN  15.0
#define PRES_MIN  950.0
#define PRES_MAX  1040.0
#define CO2_MAX   1500.0
#define VEL_MAX   8.5

DHT dht(PIN_DHT, DHT22);
WiFiClient espClient;
PubSubClient mqtt(espClient);

unsigned long ultimoEnvio = 0;

float lerPot(int pino, float minV, float maxV) {
  int raw = analogRead(pino);
  return minV + ((float)raw / 4095.0) * (maxV - minV);
}

void conectarMQTT() {
  while (!mqtt.connected()) {
    // Trava de segurança: se o WiFi cair, tenta reconectar primeiro
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("[Aviso] WiFi desconectado. Aguardando rede...");
      delay(1000);
      return; 
    }

    Serial.print("[MQTT] Conectando ao FIWARE...");
    if (mqtt.connect("dragon-esp32")) {
      Serial.println(" OK!");
    } else {
      Serial.print(" falhou rc=");
      Serial.print(mqtt.state());
      Serial.println(" - tentando em 2s");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1500);

  Serial.println();
  Serial.println("====================================");
  Serial.println(" DRAGON CAPSULE - TELEMETRIA");
  Serial.println(" FIAP - GS 2026 - Edge Computing");
  Serial.println("====================================");

  pinMode(PIN_LED_OK, OUTPUT);
  pinMode(PIN_LED_ALERT, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  dht.begin();

  Serial.print("[WiFi] Conectando");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
  }
  Serial.println(" CONECTADO!");
  Serial.print("[WiFi] IP: ");
  Serial.println(WiFi.localIP());

  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  conectarMQTT();

  digitalWrite(PIN_LED_OK, HIGH);
  Serial.println("[Sistema] PRONTO! Enviando telemetria...\n");
}

void loop() {
  if (!mqtt.connected()) conectarMQTT();
  mqtt.loop();

  if (millis() - ultimoEnvio < 10000) return;
  ultimoEnvio = millis();

  // ===== Leituras =====
  float temp = dht.readTemperature();
  float umid = dht.readHumidity();
  float pres = lerPot(PIN_POT_PRES, 940.0, 1050.0);
  float co2  = lerPot(PIN_POT_CO2,  300.0, 2500.0);
  float vel  = lerPot(PIN_POT_VEL,  6.5, 9.0);

  if (isnan(temp) || isnan(umid)) {
    Serial.println("[ERRO] Falha no DHT22");
    return;
  }

  // ===== Verifica anomalias =====
  bool alerta = false;
  String motivo = "";
  if (temp > TEMP_MAX || temp < TEMP_MIN) { alerta = true; motivo += "TEMP "; }
  if (pres > PRES_MAX || pres < PRES_MIN) { alerta = true; motivo += "PRES "; }
  if (co2 > CO2_MAX)                      { alerta = true; motivo += "CO2 "; }
  if (vel > VEL_MAX)                      { alerta = true; motivo += "VEL "; }

  // ===== Aciona LEDs e buzzer =====
  if (alerta) {
    digitalWrite(PIN_LED_OK, LOW);
    digitalWrite(PIN_LED_ALERT, HIGH);
    tone(PIN_BUZZER, 1000, 200);
  } else {
    digitalWrite(PIN_LED_OK, HIGH);
    digitalWrite(PIN_LED_ALERT, LOW);
    noTone(PIN_BUZZER);
  }

  // ===== Monta payload Ultralight 2.0 =====
  char payload[120];
  snprintf(payload, sizeof(payload),
    "t|%.1f|h|%.1f|p|%.1f|c|%.0f|v|%.2f",
    temp, umid, pres, co2, vel);

  mqtt.publish(TOPIC_PUB, payload);

  // ===== Serial Monitor =====
  Serial.println("------------------------------------");
  Serial.printf(" t=%lus  |  Status: %s\n", millis()/1000, alerta ? "ALERTA!" : "NORMAL");
  Serial.printf(" Temperatura : %.1f C\n", temp);
  Serial.printf(" Umidade     : %.1f %%\n", umid);
  Serial.printf(" Pressao     : %.1f hPa\n", pres);
  Serial.printf(" CO2         : %.0f ppm\n", co2);
  Serial.printf(" Velocidade  : %.2f km/s\n", vel);
  if (alerta) Serial.println(" >> ANOMALIA: " + motivo);
  Serial.println(" Enviado FIWARE: " + String(payload));
  Serial.println("------------------------------------");
}