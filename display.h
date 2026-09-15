#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#include "config.h"


class Display
{
public:

    Display();


    void begin();

    void clear();

    void fill(
        uint16_t color
    );


    void setRotation(
        uint8_t r
    );


    void drawPixel(
        int x,
        int y,
        uint16_t color
    );


    void drawRect(
        int x,
        int y,
        int w,
        int h,
        uint16_t color
    );


    void fillRect(
        int x,
        int y,
        int w,
        int h,
        uint16_t color
    );


    void drawCircle(
        int x,
        int y,
        int r,
        uint16_t color
    );


    void drawLine(
        int x1,
        int y1,
        int x2,
        int y2,
        uint16_t color
    );


    void drawText(
        String text,
        int x,
        int y,
        int size = 1
    );


    void pushImage(
        int x,
        int y,
        int w,
        int h,
        uint16_t* data
    );


private:

    Adafruit_ST7735 tft;
};


extern Display display;

#endif
