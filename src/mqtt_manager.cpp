#include "mqtt_manager.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "wifi_manager.h"

WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

unsigned long lastMqttCheck = 0;
MqttCallback userCallback = nullptr;

void internalCallback(char* topic, byte* payload, unsigned int length) {
  String p = "";
  for (int i = 0; i < length; i++) p += (char)payload[i];
  if (userCallback) userCallback(String(topic), p);
}

void mqttInit(MqttCallback cb) {
  userCallback = cb;
  secureClient.setInsecure(); // No certificate checking for simplicity
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(internalCallback);
}

void mqttTask() {
  if (!isWifiConnected()) return;

  if (!mqttClient.connected()) {
    if (millis() - lastMqttCheck > 5000) {
      lastMqttCheck = millis();
      
      String clientId = "ESP32_SmartGarden_";
      clientId += String(random(0xffff), HEX);
      
      // LWT: topic = garden/status, payload = offline, qos = 1, retain = true
      if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASS, "garden/status", 1, true, "offline")) {
        // Successfully connected
        mqttClient.publish("garden/status", "online", true); // Publish online status
        
        // Subscribe to commands
        mqttClient.subscribe("garden/cmd/mode");
        mqttClient.subscribe("garden/cmd/fan");
        mqttClient.subscribe("garden/cmd/led");
        mqttClient.subscribe("garden/cmd/heater");
        mqttClient.subscribe("garden/cmd/pump");
        mqttClient.subscribe("garden/cmd/mist");
      }
    }
  } else {
    mqttClient.loop();
  }
}

bool isMqttConnected() {
  return mqttClient.connected();
}

void mqttPublishTelemetry(PayloadNode1 n1, float light, float soil, ControlMode mode, bool fan, bool led, bool heater, bool pump, bool mist) {
  if (!isMqttConnected()) return;
  
  String mStr = (mode == MODE_AUTO) ? "auto" : "manual";
  
  String payload = "{\"temp\":" + String(n1.temp) + 
                   ",\"hum\":" + String(n1.hum) + 
                   ",\"motion\":" + String(n1.motion ? "true" : "false") + 
                   ",\"light\":" + String(light) + 
                   ",\"soil\":" + String(soil) + 
                   ",\"mode\":\"" + mStr + "\"" + 
                   ",\"fan\":" + String(fan ? "true" : "false") + 
                   ",\"led\":" + String(led ? "true" : "false") + 
                   ",\"heater\":" + String(heater ? "true" : "false") + 
                   ",\"pump\":" + String(pump ? "true" : "false") + 
                   ",\"mist\":" + String(mist ? "true" : "false") + "}";
                   
  mqttClient.publish("garden/telemetry", payload.c_str());
}
