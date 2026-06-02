#ifndef OLED_SSD1306_H
#define OLED_SSD1306_H

#include <stdint.h>

#define SSD1306_I2C_ADDR 0x3C

void SSD1306_Init(void);
void SSD1306_Clear(void);
void SSD1306_DisplayOn(void);
void SSD1306_DisplayOff(void);
void SSD1306_SetCursor(uint8_t x, uint8_t y);
void SSD1306_WriteChar(char ch);
void SSD1306_WriteString(const char* str);

#endif // OLED_SSD1306_H
