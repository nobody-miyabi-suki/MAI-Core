#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

class Display
{
public:
    Display();
    void begin();

    void fill(uint16_t color);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color);
    void fillCircle(int16_t x, int16_t y, int16_t r, uint16_t color);
    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
    void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

    // نسخهٔ اصلاح‌شده بدون const
    void pushImage(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t* data);

    void drawText(String text, int16_t x, int16_t y, uint8_t size);
    void drawText(const char* text, int16_t x, int16_t y, uint8_t size);

    void setRotation(uint8_t rotation);
    int16_t width();
    int16_t height();

    Adafruit_ST7735& raw();

private:
    Adafruit_ST7735 tft;
};

extern Display display;
#endif
