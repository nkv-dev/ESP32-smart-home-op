#include "lcd.h"

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

void lcd_init() {
    Wire.begin(SDA_PIN, SCL_PIN);
    lcd.init();
    lcd.backlight();
    lcd_clear();
}

void lcd_clear() {
    lcd.clear();
}

void lcd_print(const char* text) {
    lcd.print(text);
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
    lcd.setCursor(col, row);
}

void lcd_print_at(const char* text, uint8_t col, uint8_t row) {
    lcd.setCursor(col, row);
    lcd.print(text);
}
