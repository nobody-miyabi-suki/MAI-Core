#include "display.h"
#include "config.h"

#ifndef TFT_CS
#define TFT_CS 5
#endif
#ifndef TFT_DC
#define TFT_DC 2
#endif
#ifndef TFT_RST
#define TFT_RST 4
#endif

Display display;

Display::Display()
    : tft(TFT_CS, TFT_DC, TFT_RST)
{
}

void Display::begin()
{
    Serial.println();
    Serial.println("====================");
    Serial.println("DISPLAY");
    Serial.println("====================");

    tft.initR(INITR_BLACKTAB);
    tft.setRotation(0);
    tft.fillScreen(ST77XX_BLACK);

    Serial.println("ST7735 READY");
    Serial.print("WIDTH: ");
    Serial.println(tft.width());
    Serial.print("HEIGHT: ");
    Serial.println(tft.height());
}

void Display::fill(uint16_t color) {
    tft.fillScreen(color);
}

void Display::drawPixel(int16_t x, int16_t y, uint16_t color) {
    tft.drawPixel(x, y, color);
}

void Display::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    tft.drawLine(x0, y0, x1, y1, color);
}

void Display::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    tft.drawRect(x, y, w, h, color);
}

void Display::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    tft.fillRect(x, y, w, h, color);
}

void Display::drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color) {
    tft.drawCircle(x, y, r, color);
}

void Display::fillCircle(int16_t x, int16_t y, int16_t r, uint16_t color) {
    tft.fillCircle(x, y, r, color);
}

void Display::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    tft.drawTriangle(x0, y0, x1, y1, x2, y2, color);
}

void Display::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    tft.fillTriangle(x0, y0, x1, y1, x2, y2, color);
}

// ========== تابع سریع و اصلاح‌شده (مثل V2.1) ==========
void Display::pushImage(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t* data)
{
    if (data == nullptr) return;

    tft.startWrite();
    tft.setAddrWindow(x, y, w, h);               // عرض و ارتفاع واقعی
    tft.writePixels(data, (uint32_t)w * h, true); // true = bigEndian
    tft.endWrite();
}

void Display::drawText(String text, int16_t x, int16_t y, uint8_t size) {
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(size);
    tft.setCursor(x, y);
    tft.print(text);
}

void Display::drawText(const char* text, int16_t x, int16_t y, uint8_t size) {
    if (text == nullptr) return;
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(size);
    tft.setCursor(x, y);
    tft.print(text);
}

void Display::setRotation(uint8_t rotation) {
    tft.setRotation(rotation);
}

int16_t Display::width() {
    return tft.width();
}

int16_t Display::height() {
    return tft.height();
}

Adafruit_ST7735& Display::raw() {
    return tft;
}
