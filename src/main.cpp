/**
 * ===================================================================
 * ESP32 Smart Switchboard - Web Controlled Home Automation
 * ===================================================================
 * 
 * Project: ESP32 Smart Home Controller
 * Description: Web-based relay control with sensor monitoring
 * Author: Nitesh
 * Date: 2026
 * 
 * Hardware:
 * - ESP32-WROOM-32E
 * - 4-Channel Relay Module (10A)
 * - DHT11 Temperature & Humidity
 * - MQ-3 Gas Sensor
 * - PIR HC-SR501 Motion Sensor
 * - ZMPT101B Voltage Sensor
 * - ACS712 Current Sensor (20A)
 * - LCD 16x2 I2C Display
 * 
 * ===================================================================
 */

#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

// ===================================================================
// WIFI CONFIGURATION
// ===================================================================
// UPDATE THESE WITH YOUR WIFI CREDENTIALS
const char* ssid = "Your_WiFi_SSID";
const char* password = "Your_WiFi_Password";

// ===================================================================
// HARDWARE PIN CONFIGURATION
// ===================================================================
// I2C LCD Display
// Address: 0x27, Size: 16x2
// I2C Pins: SDA=GPIO 21, SCL=GPIO 22
LiquidCrystal_I2C lcd(0x27, 16, 2);

// DHT11 Temperature & Humidity Sensor
// Data Pin: GPIO 4
#define DHT_PIN 4
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

// Relay Module (4-channel, 5V activated)
// IN1: GPIO 25, IN2: GPIO 26, IN3: GPIO 27, IN4: GPIO 14
// Note: Relay module uses INVERTED logic (LOW = ON, HIGH = OFF)
#define RELAY_1 25
#define RELAY_2 26
#define RELAY_3 27
#define RELAY_4 14

// MQ-3 Gas Sensor (Analog)
// Analog Pin: GPIO 34
#define MQ3_PIN 34

// PIR HC-SR501 Motion Sensor (Digital)
// Digital Pin: GPIO 35
#define PIR_PIN 35

// ZMPT101B Voltage Sensor (Analog)
// Analog Pin: GPIO 32
#define VOLTAGE_PIN 32

// ACS712 Current Sensor (Analog)
// Analog Pin: GPIO 33
#define CURRENT_PIN 33

// ===================================================================
// GLOBAL VARIABLES
// ===================================================================
// Relay states (1 = ON, 0 = OFF)
bool relayStates[4] = {0, 0, 0, 0};

// Sensor readings
float temperature = 0;
float humidity = 0;
int gasLevel = 0;
bool motionDetected = false;
float voltage = 0;
float current = 0;
float power = 0;

// Async web server on port 80
AsyncWebServer server(80);

// ===================================================================
// SENSOR FUNCTIONS
// ===================================================================
void readDHTSensor() {
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
}

void readGasSensor() {
    int rawValue = analogRead(MQ3_PIN);
    gasLevel = map(rawValue, 0, 4095, 0, 100);
}

void readPIRSensor() {
    motionDetected = digitalRead(PIR_PIN);
}

void readVoltageSensor() {
    int rawValue = analogRead(VOLTAGE_PIN);
    float voltageRatio = (float)rawValue / 4095.0;
    voltage = voltageRatio * 250.0;
    if (voltage < 10) voltage = 0;
}

void readCurrentSensor() {
    int rawValue = analogRead(CURRENT_PIN);
    float voltageOut = (rawValue / 4095.0) * 3.3;
    float curAmps = ((voltageOut - 2.5) / 0.185);
    current = curAmps > 0 ? curAmps : 0;
}

void calculatePower() {
    power = voltage * current;
}

// ===================================================================
// RELAY FUNCTIONS
// ===================================================================
void setRelayState(int relayNum, bool state) {
    if (relayNum < 1 || relayNum > 4) return;
    
    relayStates[relayNum - 1] = state;
    
    int pin;
    switch(relayNum) {
        case 1: pin = RELAY_1; break;
        case 2: pin = RELAY_2; break;
        case 3: pin = RELAY_3; break;
        case 4: pin = RELAY_4; break;
    }
    
    digitalWrite(pin, state ? LOW : HIGH);
}

void initRelays() {
    pinMode(RELAY_1, OUTPUT);
    pinMode(RELAY_2, OUTPUT);
    pinMode(RELAY_3, OUTPUT);
    pinMode(RELAY_4, OUTPUT);
    
    digitalWrite(RELAY_1, HIGH);
    digitalWrite(RELAY_2, HIGH);
    digitalWrite(RELAY_3, HIGH);
    digitalWrite(RELAY_4, HIGH);
}

// ===================================================================
// LCD DISPLAY
// ===================================================================
void updateLCD() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print((int)temperature);
    lcd.print("C H:");
    lcd.print((int)humidity);
    lcd.print("%");
    
    lcd.setCursor(0, 1);
    if (motionDetected) {
        lcd.print("Motion: YES");
    } else {
        lcd.print("Motion: NO ");
    }
    lcd.print(" G:");
    lcd.print(gasLevel);
}

// ===================================================================
// WEB SERVER HANDLERS
// ===================================================================
String getIndexHTML() {
    String html = R"rawl(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Smart Switchboard</title>
    <style>
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body { font-family: 'Segoe UI', sans-serif; background: #1a1a2e; color: #eee; padding: 20px; }
        .container { max-width: 800px; margin: 0 auto; }
        h1 { text-align: center; color: #00d4ff; margin-bottom: 30px; }
        
        .card { background: #16213e; border-radius: 15px; padding: 20px; margin-bottom: 20px; box-shadow: 0 4px 15px rgba(0,0,0,0.3); }
        .card h2 { color: #00d4ff; margin-bottom: 15px; font-size: 1.2rem; }
        
        .relay-grid { display: grid; grid-template-columns: repeat(2, 1fr); gap: 15px; }
        .relay-item { display: flex; align-items: center; justify-content: space-between; background: #0f3460; padding: 15px; border-radius: 10px; }
        .relay-item span { font-weight: bold; }
        
        .switch { position: relative; width: 60px; height: 30px; }
        .switch input { opacity: 0; width: 0; height: 0; }
        .slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background: #e94560; border-radius: 30px; transition: 0.3s; }
        .slider:before { content: ""; position: absolute; height: 24px; width: 24px; left: 3px; bottom: 3px; background: white; border-radius: 50%; transition: 0.3s; }
        input:checked + .slider { background: #00d4ff; }
        input:checked + .slider:before { transform: translateX(30px); }
        
        .sensor-grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 15px; }
        .sensor-item { background: #0f3460; padding: 15px; border-radius: 10px; text-align: center; }
        .sensor-item .value { font-size: 1.5rem; font-weight: bold; color: #00d4ff; }
        .sensor-item .label { font-size: 0.8rem; color: #888; margin-top: 5px; }
        
        .status { text-align: center; padding: 10px; margin-top: 20px; color: #888; font-size: 0.9rem; }
        
        @media (max-width: 600px) {
            .relay-grid, .sensor-grid { grid-template-columns: 1fr; }
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>ESP32 Smart Switchboard</h1>
        
        <div class="card">
            <h2>Relay Control</h2>
            <div class="relay-grid">
                <div class="relay-item">
                    <span>Relay 1</span>
                    <label class="switch">
                        <input type="checkbox" id="r1" onchange="toggleRelay(1)">
                        <span class="slider"></span>
                    </label>
                </div>
                <div class="relay-item">
                    <span>Relay 2</span>
                    <label class="switch">
                        <input type="checkbox" id="r2" onchange="toggleRelay(2)">
                        <span class="slider"></span>
                    </label>
                </div>
                <div class="relay-item">
                    <span>Relay 3</span>
                    <label class="switch">
                        <input type="checkbox" id="r3" onchange="toggleRelay(3)">
                        <span class="slider"></span>
                    </label>
                </div>
                <div class="relay-item">
                    <span>Relay 4</span>
                    <label class="switch">
                        <input type="checkbox" id="r4" onchange="toggleRelay(4)">
                        <span class="slider"></span>
                    </label>
                </div>
            </div>
        </div>
        
        <div class="card">
            <h2>Sensor Readings</h2>
            <div class="sensor-grid">
                <div class="sensor-item">
                    <div class="value" id="temp">--</div>
                    <div class="label">Temperature (C)</div>
                </div>
                <div class="sensor-item">
                    <div class="value" id="humidity">--</div>
                    <div class="label">Humidity (%)</div>
                </div>
                <div class="sensor-item">
                    <div class="value" id="gas">--</div>
                    <div class="label">Gas Level (%)</div>
                </div>
                <div class="sensor-item">
                    <div class="value" id="motion">--</div>
                    <div class="label">Motion</div>
                </div>
                <div class="sensor-item">
                    <div class="value" id="voltage">--</div>
                    <div class="label">Voltage (V)</div>
                </div>
                <div class="sensor-item">
                    <div class="value" id="current">--</div>
                    <div class="label">Current (A)</div>
                </div>
            </div>
        </div>
        
        <div class="status">Auto-refresh every 2 seconds</div>
    </div>
    
    <script>
        function toggleRelay(num) {
            var state = document.getElementById('r' + num).checked ? 1 : 0;
            fetch('/relay?num=' + num + '&state=' + state);
        }
        
        function updateSensors() {
            fetch('/sensors')
                .then(r => r.json())
                .then(data => {
                    document.getElementById('temp').textContent = data.temperature.toFixed(1);
                    document.getElementById('humidity').textContent = data.humidity.toFixed(1);
                    document.getElementById('gas').textContent = data.gasLevel;
                    document.getElementById('motion').textContent = data.motion ? 'YES' : 'NO';
                    document.getElementById('voltage').textContent = data.voltage.toFixed(1);
                    document.getElementById('current').textContent = data.current.toFixed(2);
                    
                    document.getElementById('r1').checked = data.relay1;
                    document.getElementById('r2').checked = data.relay2;
                    document.getElementById('r3').checked = data.relay3;
                    document.getElementById('r4').checked = data.relay4;
                });
        }
        
        setInterval(updateSensors, 2000);
        updateSensors();
    </script>
</body>
</html>)rawl";
    return html;
}

// ===================================================================
// SETUP
// ===================================================================
void setup() {
    Serial.begin(115200);
    Serial.println("\n\n========================================");
    Serial.println("ESP32 Smart Switchboard Starting...");
    Serial.println("========================================");
    
    // Initialize I2C
    Wire.begin(21, 22);
    
    // Initialize LCD
    lcd.begin(16, 2);
    lcd.backlight();
    lcd.clear();
    lcd.print("Smart Switch");
    lcd.setCursor(0, 1);
    lcd.print("Initializing...");
    
    // Initialize DHT
    dht.begin();
    
    // Initialize relays
    initRelays();
    
    // Configure analog pins
    pinMode(MQ3_PIN, INPUT);
    pinMode(PIR_PIN, INPUT);
    pinMode(VOLTAGE_PIN, INPUT);
    pinMode(CURRENT_PIN, INPUT);
    
    // Connect to WiFi
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi Connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        
        lcd.clear();
        lcd.print("IP: ");
        lcd.print(WiFi.localIP().toString());
    } else {
        Serial.println("\nWiFi Connection Failed!");
        lcd.clear();
        lcd.print("WiFi Failed!");
    }
    
    // Web server routes
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", getIndexHTML());
    });
    
    server.on("/sensors", HTTP_GET, [](AsyncWebServerRequest *request) {
        String json = "{";
        json += "\"temperature\":" + String(temperature) + ",";
        json += "\"humidity\":" + String(humidity) + ",";
        json += "\"gasLevel\":" + String(gasLevel) + ",";
        json += "\"motion\":" + String(motionDetected ? "true" : "false") + ",";
        json += "\"voltage\":" + String(voltage) + ",";
        json += "\"current\":" + String(current) + ",";
        json += "\"relay1\":" + String(relayStates[0] ? "true" : "false") + ",";
        json += "\"relay2\":" + String(relayStates[1] ? "true" : "false") + ",";
        json += "\"relay3\":" + String(relayStates[2] ? "true" : "false") + ",";
        json += "\"relay4\":" + String(relayStates[3] ? "true" : "false");
        json += "}";
        request->send(200, "application/json", json);
    });
    
    server.on("/relay", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("num") && request->hasParam("state")) {
            int num = request->getParam("num")->value().toInt();
            int state = request->getParam("state")->value().toInt();
            setRelayState(num, state == 1);
            request->send(200, "text/plain", "OK");
        } else {
            request->send(400, "text/plain", "Bad Request");
        }
    });
    
    server.begin();
    Serial.println("Web server started!");
    Serial.println("========================================");
}

// ===================================================================
// LOOP
// ===================================================================
void loop() {
    readDHTSensor();
    readGasSensor();
    readPIRSensor();
    readVoltageSensor();
    readCurrentSensor();
    calculatePower();
    updateLCD();
    
    Serial.print("Temp: ");
    Serial.print(temperature);
    Serial.print("C, Humidity: ");
    Serial.print(humidity);
    Serial.print("%, Gas: ");
    Serial.print(gasLevel);
    Serial.print(", Motion: ");
    Serial.print(motionDetected ? "YES" : "NO");
    Serial.print(", V: ");
    Serial.print(voltage);
    Serial.print("V, I: ");
    Serial.print(current);
    Serial.print("A");
    Serial.println();
    
    delay(2000);
}