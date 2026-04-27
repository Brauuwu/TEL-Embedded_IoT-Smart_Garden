#include <ESP8266WiFi.h>
#include <SPI.h>
#include <RF24.h>
#include <DHT.h>

#define DHTPIN D8
#define DHTTYPE DHT11
#define PIR_PIN D4

#define NRF_CE D3
#define NRF_CSN D0

DHT dht(DHTPIN, DHTTYPE);
RF24 radio(NRF_CE, NRF_CSN);

const byte addrWrite[6] = "NODE1";

struct PayloadNode1 {
  float temp;
  float hum;
  bool motion;
};

PayloadNode1 payload;

void setup() {
  Serial.begin(115200);
  
  // Disable Wi-Fi to save power since we only use NRF
  WiFi.mode(WIFI_OFF);
  
  pinMode(PIR_PIN, INPUT);
  dht.begin();
  
  radio.begin();
  radio.openWritingPipe(addrWrite);
  radio.stopListening();
  
  Serial.println("Node 1 (Sensors) Started");
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (!isnan(h) && !isnan(t)) {
    payload.temp = t;
    payload.hum = h;
  }
  
  payload.motion = digitalRead(PIR_PIN);
  
  radio.write(&payload, sizeof(payload));
  
  Serial.print("Sent -> Temp: "); Serial.print(payload.temp);
  Serial.print(" | Hum: "); Serial.print(payload.hum);
  Serial.print(" | Motion: "); Serial.println(payload.motion);
  
  delay(1000); // Send data every 1 second
}
