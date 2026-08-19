#include "media.h"

#include "display.h"


Media media;



void Media::begin()
{

    Serial.println(
        "Media Ready"
    );

}



// ============================
// IMAGE
// ============================

bool Media::showImage(const char* path)
{

    gifPlaying = false;


    if(gifFile)
        gifFile.close();



    File file = SPIFFS.open(
        path,
        "r"
    );


    if(!file)
    {

        Serial.println(
            "IMAGE OPEN FAILED"
        );

        return false;

    }



    uint16_t width;
    uint16_t height;



    file.read(
        (uint8_t*)&width,
        2
    );


    file.read(
        (uint8_t*)&height,
        2
    );



    Serial.print(
        "IMAGE SIZE: "
    );


    Serial.print(width);

    Serial.print("x");

    Serial.println(height);




    bool validHeader = false;



    if(
        (width == 128 && height == 160) ||
        (width == 160 && height == 128)
    )
    {
        validHeader = true;
    }



    if(!validHeader)
    {

        file.seek(0);


        width = 128;
        height = 160;

    }



    if(width == 160)
        display.setRotation(1);
    else
        display.setRotation(0);



    uint16_t line[160];



    display.clear();



    for(
        int y=0;
        y<height;
        y++
    )
    {

        if(
            file.read(
                (uint8_t*)line,
                width*2
            )
            != width*2
        )
        {

            Serial.println(
                "IMAGE DATA ERROR"
            );


            file.close();

            return false;

        }



        display.pushImage(
            0,
            y,
            width,
            1,
            line
        );


    }



    file.close();



    Serial.println(
        "IMAGE DISPLAYED"
    );


    return true;

}






// ============================
// GIF START
// ============================


bool Media::playGIF(const char* path)
{

    if(gifFile)
        gifFile.close();



    gifFile = SPIFFS.open(
        path,
        "r"
    );



    if(!gifFile)
    {

        Serial.println(
            "GIF OPEN FAILED"
        );

        return false;

    }



    frameCount =
    gifFile.size()
    /
    (128 * 160 * 2);



    currentFrame = 0;



    Serial.print(
        "GIF FRAMES: "
    );

    Serial.println(
        frameCount
    );



    gifPlaying = true;



    lastFrame = millis();



    drawFrame();



    Serial.println(
        "GIF START"
    );


    return true;

}






// ============================
// LOOP UPDATE
// ============================


void Media::update()
{

    if(!gifPlaying)
        return;



    if(
        millis()-lastFrame
        <
        frameDelay
    )
        return;



    lastFrame = millis();



    drawFrame();

}





// ============================
// DRAW ONE FRAME
// ============================


void Media::drawFrame()
{


    uint16_t line[128];



    for(
        int y=0;
        y<160;
        y++
    )
    {


        if(
            gifFile.read(
                (uint8_t*)line,
                256
            )
            !=256
        )
        {

            gifFile.seek(0);

            currentFrame = 0;

            return;

        }



        display.pushImage(
            0,
            y,
            128,
            1,
            line
        );


    }



    currentFrame++;



    if(
        currentFrame >= frameCount
    )
    {

        gifFile.seek(0);

        currentFrame = 0;

    }


}
