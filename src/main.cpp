#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include "config.h"
#include "state_machine.h"
#include "display.h"
#include "sensors.h"
#include "nrf.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "app_webserver.h"
#include "buzzer.h"

PayloadNode1 node1Data = {0.0, 0.0, false};
ControlMode currentMode = MODE_AUTO;
bool fanState = false;
bool ledState = false;
bool heaterState = false;
bool pumpState = false;
bool mistState = false;

// MQTT Callback to handle manual commands
void mqttCallback(String topic, String payload) {
  if (topic == "garden/cmd/mode") {
    if (payload == "auto") currentMode = MODE_AUTO;
    else if (payload == "manual") currentMode = MODE_MANUAL;
  }
  else if (currentMode == MODE_MANUAL) {
    bool state = (payload == "on" || payload == "true" || payload == "1");
    if (topic == "garden/cmd/fan") fanState = state;
    else if (topic == "garden/cmd/led") ledState = state;
    else if (topic == "garden/cmd/heater") heaterState = state;
    else if (topic == "garden/cmd/pump") pumpState = state;
    else if (topic == "garden/cmd/mist") mistState = state;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(BTN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  
  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);
  digitalWrite(RGB_R, LOW);
  digitalWrite(RGB_G, LOW);
  digitalWrite(RGB_B, LOW);

  displayInit();
  sensorsInit();
  nrfInit();
  wifiInit();
  mqttInit(mqttCallback);
  buzzerInit();

  changeState(STATE_BOOT);
}

void loop() {

  // 👉 BOOT BUTTON → AP MODE
  if (digitalRead(BTN) == LOW && getState() != STATE_AP) {
    startAP();
    webInit();
    changeState(STATE_AP);
  }

  switch (getState()) {

    case STATE_BOOT: {
      static bool drawn = false;
      if (!drawn) {
        drawBoot();
        drawn = true;
      }
      if (buzzerStartup()) {
        changeState(STATE_NORMAL);
      }
      break;
    }

    case STATE_NORMAL: {
      static unsigned long lastDraw = 0;
      static unsigned long lastPub = 0;
      static unsigned long lastNrfNode2 = 0;
      
      wifiTask();
      mqttTask();

      // Read local sensors
      float light = readLight();
      float soil  = readSoil();

      // Read NRF Node 1 (Sensors)
      if (nrfAvailable()) {
        nrfRead(node1Data);
      }

      // ==== AUTO / MANUAL LOGIC ====
      if (currentMode == MODE_AUTO) {
        // LED on Node 2: On if light < 50 lux AND motion detected
        ledState = (light < 50.0 && node1Data.motion);

        // Fan (Relay) on ESP32: On if temp > 30°C
        fanState = (node1Data.temp > 30.0);
        
        // RGB Actuators Logic
        heaterState = (node1Data.temp < 22.0); // Red: Heating
        pumpState = (soil < 40.0);             // Green: Watering
        mistState = (node1Data.hum < 50.0);    // Blue: Humidifier
      }

      // Apply Local Actuators
      digitalWrite(RELAY_PIN, fanState ? HIGH : LOW);
      digitalWrite(RGB_R, heaterState ? HIGH : LOW);
      digitalWrite(RGB_G, pumpState ? HIGH : LOW);
      digitalWrite(RGB_B, mistState ? HIGH : LOW);
      
      buzzerAlert(node1Data.temp, soil);

      // Write to Node 2 (Actuators) over NRF24 (every 1 second)
      if (millis() - lastNrfNode2 > 1000) {
        lastNrfNode2 = millis();
        PayloadNode2 n2;
        n2.temp = node1Data.temp;
        n2.hum = node1Data.hum;
        n2.light = light;
        n2.soil = soil;
        n2.ledState = ledState;
        nrfWriteNode2(n2);
      }

      // Update TFT Display
      if (millis() - lastDraw > 500) {
        lastDraw = millis();
        drawDashboard(node1Data, light, soil, isWifiConnected(), getWifiRSSI(), isMqttConnected(), currentMode, fanState, ledState, heaterState, pumpState, mistState);
      }
      
      // Publish Telemetry to MQTT
      if (millis() - lastPub > 5000) {
        lastPub = millis();
        mqttPublishTelemetry(node1Data, light, soil, currentMode, fanState, ledState, heaterState, pumpState, mistState);
      }

      break;
    }

    case STATE_AP: {
      static unsigned long lastDrawAP = 0;
      if (millis() - lastDrawAP > 1000) {
        lastDrawAP = millis();
        drawAP(WiFi.softAPIP());
      }
      webHandle();
      break;
    }
  }
}