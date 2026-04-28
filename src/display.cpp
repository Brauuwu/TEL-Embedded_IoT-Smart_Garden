#include "display.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void displayInit() {
  tft.initR(INITR_REDTAB);
  tft.setRotation(2); // Depending on screen
  tft.fillScreen(ST77XX_BLACK);
}

void drawBoot() {
  tft.fillScreen(ST77XX_BLACK);
  
  // Vẽ biểu tượng mầm cây (Sprout)
  // Chậu cây
  tft.fillRect(54, 50, 20, 15, 0x8410); // Màu Nâu
  tft.drawRect(52, 50, 24, 4, 0xA514);  // Viền chậu
  // Thân cây
  tft.drawLine(64, 50, 64, 30, ST77XX_GREEN);
  tft.drawLine(65, 50, 65, 30, ST77XX_GREEN); // Thân dày hơn 1 chút
  // Lá cây
  tft.fillCircle(58, 38, 4, ST77XX_GREEN);
  tft.fillCircle(70, 35, 4, ST77XX_GREEN);
  tft.fillCircle(64, 28, 3, ST77XX_GREEN); // Lá non trên cùng
  
  // Fancy boot text
  tft.setCursor(15, 80);
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(2);
  tft.print("SMART");
  
  tft.setCursor(20, 105);
  tft.setTextColor(ST77XX_GREEN);
  tft.print("GARDEN");
  
  tft.setTextSize(1);
  tft.setCursor(10, 140);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("Initializing...");
}

void drawDashboard(PayloadNode1 n1, float light, float soil, bool wifiOk, int rssi, bool mqttOk, ControlMode mode, bool fanOn, bool ledOn, bool heatOn, bool pumpOn, bool mistOn) {
  tft.fillScreen(ST77XX_BLACK);

  // --- 1. STATUS ---
  tft.fillRect(0, 0, 128, 14, 0x18E3); // Dark Blue
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(2, 3);
  if (wifiOk) { tft.print("WIFI "); tft.print(rssi); } 
  else { tft.setTextColor(ST77XX_RED); tft.print("NO WIFI"); }
  
  tft.setCursor(80, 3);
  if (mqttOk) { tft.setTextColor(ST77XX_GREEN); tft.print("MQTT"); } 
  else { tft.setTextColor(ST77XX_RED); tft.print("!MQTT"); }
  
  tft.drawFastHLine(0, 15, 128, ST77XX_WHITE); // Divider

  // --- 2. SENSORS ---
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(2, 20);
  tft.print("[SENSORS]");
  
  tft.setTextColor(ST77XX_ORANGE);
  tft.setCursor(5, 32); tft.print("T:"); tft.setTextColor(ST77XX_WHITE); tft.print(n1.temp, 1); tft.print("C");
  tft.setTextColor(0x07FF); // Cyan
  tft.setCursor(65, 32); tft.print("H:"); tft.setTextColor(ST77XX_WHITE); tft.print(n1.hum, 1); tft.print("%");
  
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(5, 45); tft.print("L:"); tft.setTextColor(ST77XX_WHITE); tft.print(light, 0); tft.print("lx");
  tft.setTextColor(0x8B45); // Brown
  tft.setCursor(65, 45); tft.print("S:"); tft.setTextColor(ST77XX_WHITE); tft.print(soil, 1); tft.print("%");

  tft.drawFastHLine(0, 58, 128, ST77XX_WHITE); // Divider

  // --- 3. CONTROLLER ---
  tft.setTextColor(ST77XX_MAGENTA);
  tft.setCursor(2, 63);
  tft.print("[CONTROLLER]");

  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(5, 75); tft.print("Mode:");
  if (mode == MODE_AUTO) { tft.setTextColor(ST77XX_GREEN); tft.print("AUTO"); } 
  else { tft.setTextColor(ST77XX_ORANGE); tft.print("MANUAL"); }

  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(5, 90); tft.print("PIR :");
  if (n1.motion) { tft.setTextColor(ST77XX_RED); tft.print("MOTION!"); } 
  else { tft.setTextColor(ST77XX_GREEN); tft.print("Clear"); }

  tft.drawFastHLine(0, 103, 128, ST77XX_WHITE); // Divider

  // --- 4. ACTUATORS ---
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(2, 106);
  tft.print("[ACTUATORS]");

  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(5, 118); tft.print("Fan:");
  if (fanOn) { tft.setTextColor(ST77XX_GREEN); tft.print("ON"); } 
  else { tft.setTextColor(ST77XX_RED); tft.print("OFF"); }

  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(65, 118); tft.print("LED:");
  if (ledOn) { tft.setTextColor(ST77XX_GREEN); tft.print("ON"); } 
  else { tft.setTextColor(ST77XX_RED); tft.print("OFF"); }
  
  // RGB Indicators
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(5, 135); tft.print("RGB:");
  if (heatOn) tft.fillCircle(40, 138, 4, ST77XX_RED); else tft.drawCircle(40, 138, 4, ST77XX_RED);
  if (pumpOn) tft.fillCircle(55, 138, 4, ST77XX_GREEN); else tft.drawCircle(55, 138, 4, ST77XX_GREEN);
  if (mistOn) tft.fillCircle(70, 138, 4, ST77XX_BLUE); else tft.drawCircle(70, 138, 4, ST77XX_BLUE);
}

void drawAP(IPAddress ip) {
  tft.fillScreen(ST77XX_BLUE);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 20);
  tft.print("AP MODE");

  tft.setTextSize(1);
  tft.setCursor(5, 50);
  tft.print("Wi-Fi:");
  
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(45, 50);
  tft.print("Garden_Config");

  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(5, 75);
  tft.print("Pass:");
  
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(45, 75);
  tft.print("12345678");

  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(5, 100);
  tft.print("IP Address:");
  
  tft.setTextColor(ST77XX_GREEN);
  tft.setCursor(5, 120);
  tft.print(ip);
}
