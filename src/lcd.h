#ifndef LCD_H
#define LCD_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#define LCD_ADDRESS 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

#define SDA_PIN 21
#define SCL_PIN 22

extern LiquidCrystal_I2C lcd;

void lcd_init();
void lcd_clear();
void lcd_print(const char* text);
void lcd_set_cursor(uint8_t col, uint8_t row);
void lcd_print_at(const char* text, uint8_t col, uint8_t row);

#endif
