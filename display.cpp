#include "display.h"



Display display;



void Display::begin()
{

    tft.initR(
        INITR_BLACKTAB
    );


    tft.setRotation(
        0
    );


    tft.fillScreen(
        ST77XX_BLACK
    );


    Serial.println(
        "Display Ready"
    );

}



void Display::clear()
{
    tft.fillScreen(
        ST77XX_BLACK
    );
}



void Display::fill(
    uint16_t color
)
{
    tft.fillScreen(
        color
    );
}



void Display::setRotation(
    uint8_t r
)
{
    tft.setRotation(
        r
    );
}



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


    tft.print(
        text
    );

}



void Display::pushImage(
    int x,
    int y,
    int w,
    int h,
    uint16_t *data
)
{

    tft.startWrite();


    tft.setAddrWindow(
        x,
        y,
        w,
        h
    );


    tft.writePixels(
        data,
        w*h,
        true
    );


    tft.endWrite();

}




// ===============================
// MAI CORE BOOT
// ===============================


void Display::bootAnimation(
String wifi
)
{

    clear();



    // POWER CORE EFFECT

    for(
        int r = 5;
        r <= 35;
        r += 3
    )
    {

        clear();


        drawCircle(
            64,
            55,
            r,
            ST77XX_CYAN
        );


        drawText(
            "MAI",
            45,
            45,
            2
        );


        delay(80);

    }



    delay(500);



    clear();



    // HUD


    drawRect(
        5,
        10,
        118,
        140,
        ST77XX_BLUE
    );


    drawText(
        "MAI CORE V2",
        18,
        20,
        2
    );


    drawText(
        "BOOT",
        45,
        55,
        2
    );


    delay(700);



    drawText(
        "DISPLAY  OK",
        15,
        85,
        1
    );


    delay(300);


    drawText(
        "MEDIA    OK",
        15,
        100,
        1
    );


    delay(300);


    drawText(
        "NETWORK  OK",
        15,
        115,
        1
    );


    delay(1000);



    clear();



    drawRect(
        5,
        10,
        118,
        140,
        ST77XX_GREEN
    );


    drawText(
        "WIFI",
        45,
        25,
        2
    );


    drawText(
        wifi,
        10,
        70,
        1
    );


    drawText(
        "AP: MAI-Core",
        10,
        100,
        1
    );


    drawText(
        "READY",
        42,
        125,
        2
    );


    delay(1200);



    clear();

}
