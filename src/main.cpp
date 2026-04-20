#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD Configuration
// I2C address: 0x27 (found via I2C scanner)
// Display: 16 columns x 2 rows
// ESP32 I2C pins: SDA = GPIO 21, SCL = GPIO 22
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Counter variable for demonstration
int counter = 0;

void setup() {
    // Initialize serial communication for debugging
    Serial.begin(115200);
    Serial.println("Starting LCD...");

    // Initialize I2C communication on ESP32 pins
    // SDA (Data)  -> GPIO 21
    // SCL (Clock) -> GPIO 22
    Wire.begin(21, 22);

    // Initialize the LCD display
    // Parameters: 16 columns, 2 rows
    lcd.begin(16, 2);

    // Turn on the LCD backlight
    lcd.backlight();

    // Clear any existing display content
    lcd.clear();

    // Display welcome message on first line
    lcd.print("ESP32 Ready!");

    Serial.println("LCD initialized successfully!");
}

void loop() {
    // Move cursor to beginning of second line
    lcd.setCursor(0, 1);

    // Display counter value on LCD
    lcd.print("Counter: " + String(counter));

    // Also print to serial for debugging
    Serial.println("Counter: " + String(counter));

    // Increment counter
    counter++;

    // Wait 1 second before next update
    delay(1000);
}
