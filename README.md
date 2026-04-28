# 🌱 Smart Garden Distributed IoT System

A robust, 3-Node distributed Smart Garden system built with **ESP32**, **ESP8266**, **NRF24L01**, and **Node-RED Dashboard 2.0**. This project demonstrates a hybrid edge-to-cloud architecture where local edge nodes gather environmental data and control actuators, while a central ESP32 gateway aggregates data, handles local automation logic, and bridges the system to the cloud via **HiveMQ MQTT**.

---

## 🌟 Key Features

- **3-Node Distributed Architecture**: 
  - **Node 1 (Sensors)**: Gathers Temperature, Humidity, and PIR Motion data.
  - **Node 2 (Actuators)**: Displays data on OLED and controls an LED based on lighting conditions.
- **Center Node (Gateway)**: An ESP32 that orchestrates the logic, reads local analog sensors via MCP3008, controls a Fan Relay, sounds a Buzzer, manages a multi-functional RGB LED, and manages cloud communication.
- **RGB Actuators**: An RGB LED represents physical actuators: **Red (Heater)**, **Green (Pump)**, and **Blue (Mist/Humidifier)**.
- **Auto / Manual Control**: Switch seamlessly between hardware-driven automation (Auto) and cloud-driven manual overrides (Manual).
- **Glassmorphism Web UI (AP Mode)**: On-the-fly Wi-Fi credentials provisioning. If the ESP32 loses connection or the Boot button is pressed, it broadcasts an Access Point with a sleek, responsive configuration portal.
- **Node-RED Dashboard 2.0**: A premium, responsive Dark Theme UI featuring responsive grids, dynamic gauges, status badges, and manual control switches.
- **Advanced Data Processing**: Employs `SimpleKalmanFilter` to smooth out noisy analog readings from the Soil Moisture and LDR (Light) sensors.
- **MQTT Last Will and Testament (LWT)**: Real-time tracking of the Gateway's Online/Offline status on the dashboard.
- **Rich TFT Interface**: The ESP32 features a 1.8" TFT display (ST7735) with custom sprout iconography and neatly partitioned status zones.

---

## ⚙️ Hardware Architecture & Pinouts

### 1. Central Gateway (ESP32)
*The brain of the operation. Runs the core state machine.*
- **NRF24L01**: `CE = GPIO21`, `CSN = GPIO22`
- **TFT ST7735 (SPI)**: `CS = GPIO5`, `RST = GPIO4`, `DC = GPIO2`
- **MCP3008 (SPI ADC)**: `CS = GPIO13` -> Reads LDR (CH0) and Soil Moisture (CH2)
- **Actuators**: Relay/Fan (`GPIO25`), Buzzer (`GPIO12`)
- **RGB LED Actuators**: `R = GPIO26` (Heater), `G = GPIO27` (Pump), `B = GPIO14` (Mist)
- **Inputs**: AP Mode Button (`GPIO0` - BOOT)

### 2. Node 1 - Sensors (ESP8266)
*Edge node responsible for gathering distal data.*
- **DHT11**: `D2`
- **PIR Sensor**: `D4`
- **NRF24L01**: `CE = D8`, `CSN = D0`

### 3. Node 2 - Actuators (ESP8266)
*Edge node responsible for distal actions and local display.*
- **OLED SSD1306**: `I2C (D1/D2)`
- **LED**: `D3`
- **NRF24L01**: `CE = D8`, `CSN = D0`

---

## 📂 Project Structure

```text
├── include/
│   ├── config.h           # System constants, MQTT credentials, and NRF Payloads
│   ├── display.h          # TFT UI logic
│   ├── sensors.h          # Sensor abstraction & Kalman filtering
│   └── state_machine.h    # Core finite state machine (Boot -> Normal -> AP)
├── src/
│   ├── main.cpp           # Main loop, NRF coordination, Auto/Manual logic
│   ├── app_webserver.cpp  # HTML/CSS/JS for the AP Mode Web UI
│   └── mqtt_manager.cpp   # HiveMQ Pub/Sub and LWT logic
├── extra/                 # Files for external nodes & cloud
│   ├── Node1_Sensors/     # Arduino code for ESP8266 Node 1
│   ├── Node2_Actuators/   # Arduino code for ESP8266 Node 2
│   └── NodeRED/           # flow.json for Flowfuse Dashboard 2.0
└── platformio.ini         # PlatformIO configuration & dependencies
```

---

## 🚀 Installation & Setup

### 1. Flash the ESP32 (Gateway)
1. Open this project in **VSCode + PlatformIO**.
2. Open `include/config.h` and verify your HiveMQ credentials (`MQTT_SERVER`, `MQTT_USER`, `MQTT_PASS`).
3. Connect your ESP32 and click **Upload**.

### 2. Flash the ESP8266 Nodes
1. Navigate to the `extra/Node1_Sensors/` folder and open `Node1_Sensors.ino` using the Arduino IDE.
2. Select your ESP8266 board and upload.
3. Repeat the process for `extra/Node2_Actuators/Node2_Actuators.ino`.

### 3. Setup Node-RED Dashboard
1. Ensure you have **Node-RED** installed with the `@flowfuse/node-red-dashboard` plugin.
2. Open your Node-RED workspace.
3. Click the Hamburger Menu -> **Import**.
4. Select the `extra/NodeRED/flow.json` file.
5. Deploy the flow and navigate to `/dashboard` to view your control panel.

---

## 📱 How to Use

1. **First Boot (Wi-Fi Setup)**: 
   - Upon first power-up, if no Wi-Fi networks are saved, the ESP32 will enter **AP Mode** and display an IP address on the TFT screen.
   - Connect to the Wi-Fi network `SmartGarden_AP` from your phone/laptop.
   - Visit the IP address shown on the screen, scan for your home Wi-Fi, enter the password, and hit Save. The system will reboot into Normal Mode.
2. **Dashboard Controls**:
   - By default, the system operates in **AUTO** mode (Fan triggers > 30°C, LED triggers on low light + motion).
   - Use the **Auto/Manual** switch on the dashboard to override. Once in Manual mode, use the Fan and LED toggles to control the garden remotely.
3. **Reset Wi-Fi**: Hold the **BOOT** button on the ESP32 at any time to force the system back into AP Mode to change network settings.

---
*Built with ❤️ for Embedded IoT Systems.*
