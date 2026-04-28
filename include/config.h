#pragma once

#include <Arduino.h>

// ===== WIFI & MQTT (HiveMQ Cloud) =====
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASS "YOUR_WIFI_PASS"

#define MQTT_SERVER "your-hivemq-cloud-instance.s1.eu.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_USER "HIVEMQ_USER"
#define MQTT_PASS "HIVEMQ_PASS"
#define MQTT_TOPIC "garden/sensors"

// ===== PIN =====
#define TFT_CS 5
#define TFT_DC 2
#define TFT_RST 4

#define MCP_CS 13

#define NRF_CE 21
#define NRF_CSN 22

#define BTN 0
#define BUZZER_PIN 12

#define RELAY_PIN 25
#define RGB_R 26
#define RGB_G 27
#define RGB_B 14

// ===== DATA =====
struct PayloadNode1 {
  float temp;
  float hum;
  bool motion;
};

struct PayloadNode2 {
  float temp;
  float hum;
  float light;
  float soil;
  bool ledState;
};

enum ControlMode {
  MODE_AUTO,
  MODE_MANUAL
};