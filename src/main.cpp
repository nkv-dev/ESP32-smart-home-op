#include <Arduino.h>
#include <Wire.h>
#include "lcd.h"

int counter = 0;

void setup() {
    Serial.begin(115200);
    lcd_init();
    lcd_print_at("ESP32 Ready!", 0, 0);
    Serial.println("LCD Initialized");
}

void loop() {
    counter++;
    lcd_set_cursor(0, 1);
    lcd_print("Counter: ");
    lcd.print(counter);
    Serial.println("Counter: " + String(counter));
    delay(1000);
}
