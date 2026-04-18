/**
 * ===================================================================
 * ESP32 DHT11 Humidity & Temperature Sensor with LCD Display
 * ===================================================================
 * 
 * Project: ESP32 Weather Station
 * Description: Read DHT11 sensor data and display on I2C LCD 16x2
 * Author: ESP32 Project
 * Date: 2026
 * 
 * Hardware:
 * - ESP32-WROOM-32E
 * - I2C LCD 16x2 (address 0x27)
 * - DHT11 Temperature & Humidity Sensor
 * 
 * Circuit Connections:
 * ------------------
 * LCD Module (I2C) -> ESP32
 * ------------------------
 * VCC (LCD)     -> 5V or 3.3V
 * GND (LCD)     -> GND
 * SDA (LCD)     -> GPIO 21 (I2C SDA)
 * SCL (LCD)     -> GPIO 22 (I2C SCL)
 * 
 * DHT11 Sensor -> ESP32
 * --------------------
 * VCC (DHT11)   -> 3.3V
 * GND (DHT11)   -> GND
 * DATA (DHT11)  -> GPIO 4
 * 
 * ===================================================================
 */

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ===================================================================
// LCD CONFIGURATION
// ===================================================================
// I2C LCD Display Settings
// - I2C Address: 0x27 (found via I2C scanner)
// - Display Size: 16 columns x 2 rows
// - ESP32 I2C Pins: SDA = GPIO 21, SCL = GPIO 22
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===================================================================
// DHT11 SENSOR CONFIGURATION
// ===================================================================
// DHT11 Sensor Settings
// - Data Pin: GPIO 4 (any GPIO can be used)
// - Sensor Type: DHT11 (Temperature: 0-50°C, Humidity: 20-90%)
// - DHT11 is the basic/cheap version
// - For better accuracy, use DHT22 (AM2302)
#define DHT_PIN 4          // GPIO pin connected to DHT11 DATA pin
#define DHT_TYPE DHT11    // Sensor type: DHT11, DHT21, or DHT22

// Create DHT sensor object
// Parameters: pin number, sensor type
DHT dht(DHT_PIN, DHT_TYPE);

// ===================================================================
// GLOBAL VARIABLES
// ===================================================================
// Variables to store sensor readings
float humidity = 0;        // Humidity value (0-100%)
float temperature = 0;     // Temperature value in Celsius

// ===================================================================
// SETUP FUNCTION - Runs once at startup
// ===================================================================
void setup() {
    // Initialize serial communication for debugging
    // Baud rate: 115200
    Serial.begin(115200);
    Serial.println("========================================");
    Serial.println("DHT11 + LCD Weather Station Starting...");
    Serial.println("========================================");

    // Initialize I2C communication on ESP32
    // SDA (Data Line)   -> GPIO 21
    // SCL (Clock Line) -> GPIO 22
    Wire.begin(21, 22);

    // Initialize LCD display
    // Parameters: 16 columns, 2 rows
    lcd.begin(16, 2);

    // Turn on LCD backlight
    lcd.backlight();

    // Clear any existing display content
    lcd.clear();

    // Initialize DHT11 sensor
    dht.begin();

    // Display startup message
    lcd.setCursor(0, 0);
    lcd.print("Weather Station");
    lcd.setCursor(0, 1);
    lcd.print("Initializing...");

    Serial.println("System Initialized Successfully!");
    Serial.println("========================================");
}

// ===================================================================
// LOOP FUNCTION - Runs continuously
// ===================================================================
void loop() {
    // =================================================================
    // READ SENSOR DATA
    // =================================================================
    // Read humidity from DHT11 (returns percentage 0-100%)
    humidity = dht.readHumidity();

    // Read temperature from DHT11 (returns Celsius)
    temperature = dht.readTemperature();

    // =================================================================
    // CHECK FOR SENSOR ERRORS
    // =================================================================
    // isnan() checks if value is Not-a-Number (indicates sensor error)
    // This can happen if:
    // - Sensor wiring is incorrect
    // - Sensor is not connected
    // - Sensor is malfunctioning
    if (isnan(humidity) || isnan(temperature)) {
        // Display error on LCD
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Sensor Error!");
        lcd.setCursor(0, 1);
        lcd.print("Check wiring");

        // Print error to serial
        Serial.println("ERROR: Failed to read DHT11 sensor!");
        Serial.println("Please check:");
        Serial.println("  1. VCC connected to 3.3V");
        Serial.println("  2. GND connected to GND");
        Serial.println("  3. DATA connected to GPIO 4");

        // Wait 2 seconds before retrying
        delay(2000);
        return;
    }

    // =================================================================
    // DISPLAY DATA ON LCD
    // =================================================================
    // Clear display before showing new values
    lcd.clear();

    // Line 1 (Row 0): Display Humidity
    // Position: column 0, row 0
    lcd.setCursor(0, 0);
    lcd.print("Humidity: ");
    lcd.print((int)humidity);  // Cast float to int (remove decimal)
    lcd.print("%");

    // Line 2 (Row 1): Display Temperature
    // Position: column 0, row 1
    lcd.setCursor(0, 1);
    lcd.print("Temp: ");
    lcd.print((int)temperature);  // Cast float to int
    lcd.print("C");               // Celsius symbol

    // =================================================================
    // PRINT DATA TO SERIAL MONITOR
    // =================================================================
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("%, Temperature: ");
    Serial.print(temperature);
    Serial.println("C");

    // =================================================================
    // WAIT BEFORE NEXT READING
    // =================================================================
    // DHT11 minimum reading interval: 1 second
    // We use 2 seconds to be safe
    // This also prevents display flickering
    delay(2000);
}