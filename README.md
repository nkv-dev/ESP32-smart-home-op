# ESP32 Smart Switchboard - Smart Home Controller

> **Note**: This README is a work in progress. Updates will be added as the project evolves.

---

## Project Overview

An ESP32-based local network controlled smart switchboard with relay control, environmental sensors, and power monitoring. Control your home appliances via a web browser over local WiFi network.

## Features

- **Web Interface Control**: Control 4 relays via any browser (mobile/desktop)
- **Real-time Sensor Monitoring**: View all sensor readings live
- **Temperature & Humidity**: DHT11 sensor for climate monitoring
- **Gas Detection**: MQ-3 sensor for alcohol, benzene, smoke detection
- **Motion Detection**: PIR HC-SR501 for human motion sensing
- **Power Monitoring**: ZMPT101B (voltage) + ACS712 (current) for power consumption
- **Local Display**: LCD 16x2 I2C for local status
- **Auto-refresh**: Web UI updates automatically

---

## Hardware Components

| Component | Model | Quantity | Purpose |
|-----------|-------|----------|---------|
| Microcontroller | ESP32-WROOM-32E | 1 | Main controller |
| Temperature/Humidity | DHT11 | 1 | Ambient climate |
| Gas Sensor | MQ-3 | 1 | Alcohol/smoke detection |
| Motion Sensor | PIR HC-SR501 | 1 | Motion detection |
| Voltage Sensor | ZMPT101B | 1 | AC voltage monitoring |
| Current Sensor | ACS712 (20A) | 1 | AC current monitoring |
| Relay Module | 5V 4-channel (10A) | 1 | Control 4 AC loads |
| Display | LCD 16x2 I2C (0x27) | 1 | Local status display |

---

## Pin Configuration

| Component | ESP32 GPIO | Notes |
|-----------|------------|-------|
| DHT11 Data | GPIO 4 | Digital input |
| LCD SDA | GPIO 21 | I2C data |
| LCD SCL | GPIO 22 | I2C clock |
| MQ-3 Analog | GPIO 34 | ADC input |
| PIR Data | GPIO 35 | Digital input |
| ZMPT101B | GPIO 32 | ADC input |
| ACS712 | GPIO 33 | ADC input |
| Relay IN1 | GPIO 25 | Relay 1 |
| Relay IN2 | GPIO 26 | Relay 2 |
| Relay IN3 | GPIO 27 | Relay 3 |
| Relay IN4 | GPIO 14 | Relay 4 |

---

## System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     ESP32 Web Server                        │
├─────────────────────────────────────────────────────────────┤
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐     │
│  │  Relay 1 │  │  Relay 2 │  │  Relay 3 │  │  Relay 4 │     │ 
│  │ (Light 1)│  │ (Light 2)│  │  (Fan)   │  │ (Custom) │     │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘     │
│       │             │             │             │           │
│  ┌────▼─────────────▼─────────────▼─────────────▼────┐      │
│  │              Sensor Array                         │      │
│  │  DHT11  │  MQ-3  │  PIR   │ ZMPT101B │  ACS712    │      │
│  └─────────┴────────┴────────┴──────────┴───────────┘       │
│                           │                                 │
│                    ┌──────▼──────┐                          │
│                    │ LCD 16x2    │                          │
│                    │ (I2C 0x27)  │                          │
│                    └─────────────┘                          │
└─────────────────────────────────────────────────────────────┘
                          │
                   WiFi Network
                          │
        ┌─────────────────┼─────────────────┐
        ▼                 ▼                 ▼
   ┌─────────┐       ┌─────────┐       ┌─────────┐
   │ Browser │       │ Browser │       │ Browser │
   │ Phone   │       │ Tablet  │       │ Laptop  │
   └─────────┘       └─────────┘       └─────────┘
```

---

## Getting Started

### Prerequisites

- PlatformIO IDE (VSCode extension) or Arduino IDE
- ESP32-WROOM-32E board
- USB cable for programming
- WiFi network for connectivity

### Installation

1. Clone this repository
2. Open in PlatformIO or Arduino IDE
3. Update `src/main.cpp` with your WiFi credentials:
   ```cpp
   const char* ssid = "Your_WiFi_SSID";
   const char* password = "Your_WiFi_Password";
   ```
4. Build and upload to ESP32
5. Open serial monitor (115200 baud) to see the IP address
6. Navigate to `http://<ESP32_IP>` in your browser

---

## Web Interface

The web interface provides:
- Toggle switches for 4 relays
- Real-time sensor readings display
- Temperature & Humidity
- Gas level (MQ-3)
- Motion status
- Voltage & Current
- Power consumption (calculated)

---

## Dependencies

### Required Libraries (platformio.ini)

- `ESPAsyncWebServer` - Async web server
- `Adafruit Unified Sensor` - DHT sensor
- `DHT sensor library` - DHT11
- `LiquidCrystal_I2C` - LCD display
- `ACS712` - Current sensor (optional)

---

## Power Calculation

- **Voltage**: Read from ZMPT101B (after voltage divider)
- **Current**: Read from ACS712
- **Power**: P = V × I (in Watts)

---

## Safety Warnings

> ⚠️ **WARNING**: This project involves working with mains voltage (220V/110V). Always disconnect power before working on the circuit. If you're not experienced with high-voltage electronics, please seek professional help.

- Use proper isolation between ESP32 and AC circuits
- Use optocouplers between ESP32 and relay module
- Ensure proper grounding
- Use appropriate rated components

---

## Future Enhancements

- [ ] MQTT integration for Home Assistant
- [ ] OTA (Over-the-Air) updates
- [ ] NTP time sync
- [ ] Data logging to SD card
- [ ] Push notifications
- [ ] Voice control integration
- [ ] Scheduling/timers

---

## License

MIT License

---

## Author

Nitesh - ESP32 Smart Home Project
