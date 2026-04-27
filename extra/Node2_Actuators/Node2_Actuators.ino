#include <ESP8266WiFi.h>
#include <SPI.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LED_PIN D3

#define NRF_CE D8
#define NRF_CSN D0

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RF24 radio(NRF_CE, NRF_CSN);

const byte addrRead[6] = "NODE2";

struct PayloadNode2 {
  float temp;
  float hum;
  float light;
  float soil;
  bool ledState;
};

PayloadNode2 payload;

void setup() {
  Serial.begin(115200);
  
  WiFi.mode(WIFI_OFF);
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.print("Waiting for Data...");
  display.display();
  
  radio.begin();
  radio.openReadingPipe(0, addrRead);
  radio.startListening();
  
  Serial.println("Node 2 (Actuators) Started");
}

void loop() {
  if (radio.available()) {
    radio.read(&payload, sizeof(payload));
    
    // Control LED
    digitalWrite(LED_PIN, payload.ledState ? HIGH : LOW);
    
    // Update OLED Display
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    
    display.println("--- SMART GARDEN ---");
    display.println("");
    
    display.print("Temp:  "); display.print(payload.temp, 1); display.println(" C");
    display.print("Hum:   "); display.print(payload.hum, 1); display.println(" %");
    display.print("Light: "); display.print(payload.light, 1); display.println(" lx");
    display.print("Soil:  "); display.print(payload.soil, 1); display.println(" %");
    
    display.println("");
    display.print("LED: ");
    display.print(payload.ledState ? "ON" : "OFF");
    
    display.display();
  }
}
