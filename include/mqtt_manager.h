#pragma once
#include "config.h"
#include <PubSubClient.h>

typedef void (*MqttCallback)(String topic, String payload);

void mqttInit(MqttCallback cb);
void mqttTask();
bool isMqttConnected();
void mqttPublishTelemetry(PayloadNode1 n1, float light, float soil, ControlMode mode, bool fan, bool led);
