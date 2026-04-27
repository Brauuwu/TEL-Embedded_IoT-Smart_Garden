#pragma once
#include "config.h"

void displayInit();
void drawBoot();
void drawDashboard(PayloadNode1 n1, float light, float soil, bool wifiOk, int rssi, bool mqttOk, ControlMode mode, bool fanOn, bool ledOn);
void drawAP(IPAddress ip);
