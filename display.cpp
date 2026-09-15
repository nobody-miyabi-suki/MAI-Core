#include "display.h"


Display display;


// =====================================================
// CONSTRUCTOR
// =====================================================

Display::Display()
    : tft(
        TFT_CS,
        TFT_DC,
        TFT_RST
    )
{
}


// =====================================================
// BEGIN
// =====================================================

void Display::begin()
{
    Serial.println(
        "Initializing TFT..."
    );


    pinMode(
        TFT_CS,
        OUTPUT
    );

    digitalWrite(
        TFT_CS,
        HIGH
    );


    pinMode(
        TFT_DC,
        OUTPUT
    );


    pinMode(
        TFT_RST,
        OUTPUT
    );


    tft.initR(
        INITR_BLACKTAB
    );


    tft.setRotation(
        0
    );


    tft.fillScreen(
        ST77XX_BLACK
    );


    delay(100);


    Serial.println(
        "Display Ready"
    );
}


// =====================================================
// CLEAR
// =====================================================

void Display::clear()
{
    tft.fillScreen(
        ST77XX_BLACK
    );
}


// =====================================================
// FILL
// =====================================================

void Display::fill(
    uint16_t color
)
{
    tft.fillScreen(
        color
    );
}


// =====================================================
// ROTATION
// =====================================================

void Display::setRotation(
    uint8_t r
)
{
    tft.setRotation(
        r
    );
}


// =====================================================
// PIXEL
// =====================================================

void Display::drawPixel(
    int x,
    int y,
    uint16_t color
)
{
    tft.drawPixel(
        x,
        y,
        color
    );
}


// =====================================================
// RECT
// =====================================================

void Display::drawRect(
    int x,
    int y,
    int w,
    int h,
    uint16_t color
)
{
    tft.drawRect(
        x,
        y,
        w,
        h,
        color
    );
}


// =====================================================
// FILL RECT
// =====================================================

void Display::fillRect(
    int x,
    int y,
    int w,
    int h,
    uint16_t color
)
{
    tft.fillRect(
        x,
        y,
        w,
        h,
        color
    );
}


// =====================================================
// CIRCLE
// =====================================================

void Display::drawCircle(
    int x,
    int y,
    int r,
    uint16_t color
)
{
    tft.drawCircle(
        x,
        y,
        r,
        color
    );
}


// =====================================================
// LINE
// =====================================================

void Display::drawLine(
    int x1,
    int y1,
    int x2,
    int y2,
    uint16_t color
)
{
    tft.drawLine(
        x1,
        y1,
        x2,
        y2,
        color
    );
}


// =====================================================
// TEXT
// =====================================================

void Display::drawText(
    String text,
    int x,
    int y,
    int size
)
{
    tft.setCursor(
        x,
        y
    );


    tft.setTextColor(
        ST77XX_WHITE
    );


    tft.setTextSize(
        size
    );


    tft.setTextWrap(
        false
    );


    tft.print(
        text
    );
}


// =====================================================
// PUSH IMAGE
// =====================================================

void Display::pushImage(
    int x,
    int y,
    int w,
    int h,
    uint16_t* data
)
{
    if (!data)
        return;


    tft.startWrite();


    tft.setAddrWindow(
        x,
        y,
        w,
        h
    );


    tft.writePixels(
        data,
        (uint32_t)w * h,
        true
    );


    tft.endWrite();
}
