#include "oled_ssd1306.h"
#include "i2c/i2c.h"
#include <string.h>

// 5x7 font (ASCII 32..127)
static const uint8_t font5x7[][5] = {
    // Only space (32) and 'H', 'e', 'l', 'o' for "Hello"
    {0x00,0x00,0x00,0x00,0x00}, // ' '
    {0x7F,0x08,0x08,0x08,0x7F}, // 'H' (72)
    {0x38,0x44,0x44,0x44,0x20}, // 'e' (101)
    {0x7F,0x48,0x48,0x48,0x30}, // 'l' (108)
    {0x3C,0x40,0x40,0x40,0x3C}, // 'o' (111)
};

#define WIDTH 128
#define HEIGHT 64
#define PAGES (HEIGHT/8)
static uint8_t buffer[WIDTH * PAGES] = {0};
static uint8_t cursor_x = 0, cursor_y = 0;

static void ssd1306_send_cmd(uint8_t cmd) {
    uint8_t d[2] = {0x00, cmd};
    I2C1_MasterWrite(SSD1306_I2C_ADDR, d[0], &d[1], 1);
}

static void ssd1306_send_data(uint8_t* data, uint16_t len) {
    uint8_t buf[17];
    buf[0] = 0x40;
    for (uint16_t i = 0; i < len; i += 16) {
        uint8_t l = (len - i > 16) ? 16 : (len - i);
        memcpy(&buf[1], &data[i], l);
        I2C1_MasterWrite(SSD1306_I2C_ADDR, buf[0], &buf[1], l);
    }
}

void SSD1306_Init(void) {
    I2C1_Init();
    ssd1306_send_cmd(0xAE); // Display off
    ssd1306_send_cmd(0x20); ssd1306_send_cmd(0x00); // Horizontal addressing
    ssd1306_send_cmd(0xB0); // Page 0
    ssd1306_send_cmd(0xC8); // COM scan direction
    ssd1306_send_cmd(0x00); // Low col
    ssd1306_send_cmd(0x10); // High col
    ssd1306_send_cmd(0x40); // Start line
    ssd1306_send_cmd(0x81); ssd1306_send_cmd(0x7F); // Contrast
    ssd1306_send_cmd(0xA1); // Segment remap
    ssd1306_send_cmd(0xA6); // Normal display
    ssd1306_send_cmd(0xA8); ssd1306_send_cmd(0x3F); // Multiplex
    ssd1306_send_cmd(0xA4); // Output follows RAM
    ssd1306_send_cmd(0xD3); ssd1306_send_cmd(0x00); // Display offset
    ssd1306_send_cmd(0xD5); ssd1306_send_cmd(0x80); // Clock
    ssd1306_send_cmd(0xD9); ssd1306_send_cmd(0xF1); // Precharge
    ssd1306_send_cmd(0xDA); ssd1306_send_cmd(0x12); // COM pins
    ssd1306_send_cmd(0xDB); ssd1306_send_cmd(0x40); // VCOM
    ssd1306_send_cmd(0x8D); ssd1306_send_cmd(0x14); // Charge pump
    ssd1306_send_cmd(0xAF); // Display ON
    SSD1306_Clear();
}

void SSD1306_Clear(void) {
    //memset(buffer, 0, sizeof(buffer));
    for (uint8_t page = 0; page < PAGES; page++) {
        ssd1306_send_cmd(0xB0 );//| page);
        ssd1306_send_cmd(0x00);
        ssd1306_send_cmd(0x10);
       uint8_t clear[WIDTH] = {0}; 
        ssd1306_send_data(clear, WIDTH);
    }
    cursor_x = 0; cursor_y = 0;
}

void SSD1306_DisplayOn(void) { ssd1306_send_cmd(0xAF); }
void SSD1306_DisplayOff(void) { ssd1306_send_cmd(0xAE); }

void SSD1306_SetCursor(uint8_t x, uint8_t y) {
    cursor_x = x;
    cursor_y = y;
    ssd1306_send_cmd(0xB0 | y);
    ssd1306_send_cmd(0x00 | (x & 0x0F));
    ssd1306_send_cmd(0x10 | (x >> 4));
}

void SSD1306_WriteChar(char ch) {
    uint8_t data[6] = {0};
    if (ch == 'H') memcpy(data, font5x7[1], 5);
    else if (ch == 'e') memcpy(data, font5x7[2], 5);
    else if (ch == 'l') memcpy(data, font5x7[3], 5);
    else if (ch == 'o') memcpy(data, font5x7[4], 5);
    else memcpy(data, font5x7[0], 5);
    ssd1306_send_data(data, 6);
    cursor_x += 6;
}

void SSD1306_WriteString(const char* str) {
    while (*str) SSD1306_WriteChar(*str++);
}
