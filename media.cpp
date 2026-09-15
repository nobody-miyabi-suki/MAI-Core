#include "media.h"

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

#include "config.h"
#include "display.h"

// =====================================================
// CONSTANTS
// =====================================================

#define IMAGE_W 128
#define IMAGE_H 160

#define IMAGE_SIZE 40960

// 4 byte header + 160x128 RGB565
#define ROTATED_IMAGE_SIZE 40964

#define ROTATED_W 160
#define ROTATED_H 128

// RGB565
#define BYTES_PER_PIXEL 2

// GIF frame size
#define GIF_FRAME_SIZE 40960

// Change if you want faster/slower GIF
#define GIF_FRAME_DELAY 40

// Buffer must NOT be huge.
// This keeps RAM usage low.
#define LINE_BUFFER_PIXELS 128

// =====================================================
// GLOBAL
// =====================================================

Media media;

// =====================================================
// CONSTRUCTOR
// =====================================================

Media::Media()
{
    playing = false;
    isGif = false;

    fileSize = 0;
    frameSize = 0;
    frameCount = 0;
    currentFrame = 0;

    lastFrameTime = 0;
    frameDelay = GIF_FRAME_DELAY;
}

// =====================================================
// BEGIN
// =====================================================

void Media::begin()
{
    Serial.println();
    Serial.println("====================");
    Serial.println("MEDIA INIT");
    Serial.println("====================");

    playing = false;

    Serial.println("Media Ready");
}

// =====================================================
// CLOSE FILE
// =====================================================

void Media::closeFile()
{
    if (mediaFile)
    {
        mediaFile.close();
    }

    playing = false;
    isGif = false;

    fileSize = 0;
    frameSize = 0;
    frameCount = 0;
    currentFrame = 0;
}

// =====================================================
// STOP
// =====================================================

void Media::stop()
{
    if (playing)
    {
        Serial.println("Stopping media...");
    }

    closeFile();

    display.fill(ST77XX_BLACK);
}

// =====================================================
// IS PLAYING
// =====================================================

bool Media::isPlaying()
{
    return playing;
}

// =====================================================
// READ 16 BIT
// =====================================================

uint16_t Media::read16(File &file)
{
    uint8_t low = file.read();
    uint8_t high = file.read();

    return ((uint16_t)high << 8) | low;
}

// =====================================================
// SHOW IMAGE
//
// RULE:
//
// 40960 = normal 128x160
//
// 40964 =
// [2 bytes width]
// [2 bytes height]
// [160x128 RGB565]
//
// => rotate to 128x160
// =====================================================

bool Media::showImage(const char* path)
{
    Serial.println();
    Serial.println("====================");
    Serial.println("MEDIA IMAGE");
    Serial.println("====================");

    Serial.print("PATH: ");
    Serial.println(path);

    File file = SD.open(path, FILE_READ);

    if (!file)
    {
        Serial.println("IMAGE OPEN FAILED");
        return false;
    }

    size_t fileSize = file.size();

    Serial.print("FILE SIZE: ");
    Serial.println(fileSize);

    // =================================================
    // NORMAL IMAGE
    // 128 x 160 RGB565
    // 40960 bytes
    // =================================================

    if (fileSize == 40960)
    {
        Serial.println("FORMAT: 128x160");

        display.setRotation(0);
        display.fill(ST77XX_BLACK);

        static uint16_t line[128];

        for (int y = 0; y < 160; y++)
        {
            size_t readBytes =
                file.read(
                    (uint8_t*)line,
                    128 * 2
                );

            if (readBytes != 256)
            {
                Serial.println("IMAGE READ ERROR");
                file.close();
                return false;
            }

            display.pushImage(
                0,
                y,
                128,
                1,
                line
            );
        }

        file.close();

        Serial.println("IMAGE DISPLAYED");
        return true;
    }

    // =================================================
    // ROTATED IMAGE
    // 160 x 128 RGB565
    // 40964 bytes = 4-byte header + 40960
    // =================================================

    if (fileSize == 40964)
    {
        Serial.println("FORMAT: 160x128 HEADER IMAGE");
        Serial.println("ROTATING TO 128x160");

        uint8_t header[4];

        if (file.read(header, 4) != 4)
        {
            Serial.println("HEADER READ FAILED");
            file.close();
            return false;
        }

        uint16_t headerWidth =
            header[0] |
            ((uint16_t)header[1] << 8);

        uint16_t headerHeight =
            header[2] |
            ((uint16_t)header[3] << 8);

        Serial.print("HEADER WIDTH: ");
        Serial.println(headerWidth);

        Serial.print("HEADER HEIGHT: ");
        Serial.println(headerHeight);

        if (
            headerWidth != 160 ||
            headerHeight != 128
        )
        {
            Serial.println("INVALID ROTATED HEADER");
            file.close();
            return false;
        }

        display.setRotation(0);
        display.fill(ST77XX_BLACK);

        // -------------------------------------------------
        // 160 x 128 source
        // 128 x 160 destination
        //
        // Rotate 90 degrees clockwise:
        //
        // destination X = 127 - source Y
        // destination Y = source X
        // -------------------------------------------------

        static uint16_t sourceLine[160];

        for (int sourceY = 0;
             sourceY < 128;
             sourceY++)
        {
            size_t readBytes =
                file.read(
                    (uint8_t*)sourceLine,
                    160 * 2
                );

            if (readBytes != 320)
            {
                Serial.println("ROTATED IMAGE READ ERROR");
                file.close();
                return false;
            }

            for (int sourceX = 0;
                 sourceX < 160;
                 sourceX++)
            {
                uint16_t pixel =
                    sourceLine[sourceX];

                int destX =
                    127 - sourceY;

                int destY =
                    sourceX;

                display.drawPixel(
                    destX,
                    destY,
                    pixel
                );
            }
        }

        file.close();

        Serial.println("ROTATED IMAGE DISPLAYED");
        return true;
    }

    Serial.println("UNKNOWN IMAGE FORMAT");

    file.close();
    return false;
}

// =====================================================
// NORMAL IMAGE
//
// 40960 bytes
// 128 x 160 x 2
// =====================================================

bool Media::showNormalImage(File &file)
{
    if (!file)
    {
        return false;
    }

    if (file.size() != IMAGE_SIZE)
    {
        return false;
    }

    // -----------------------------------------------
    // Always start at beginning
    // -----------------------------------------------

    file.seek(0);

    display.fill(ST77XX_BLACK);

    // -----------------------------------------------
    // Line buffer
    // -----------------------------------------------

    uint16_t line[LINE_BUFFER_PIXELS];

    for (int y = 0; y < IMAGE_H; y++)
    {
        // Read one complete 128 pixel line
        size_t bytesRead =
            file.read(
                (uint8_t*)line,
                IMAGE_W * 2
            );

        if (bytesRead != IMAGE_W * 2)
        {
            Serial.println("IMAGE READ ERROR");
            return false;
        }

        // -------------------------------------------
        // RGB565
        //
        // bigEndian=true because the converted
        // files are already MSB first.
        // -------------------------------------------

        display.pushImage(
            0,
            y,
            IMAGE_W,
            1,
            line
        );
    }

    Serial.println("NORMAL IMAGE DISPLAYED");

    return true;
}

// =====================================================
// ROTATED IMAGE
//
// FILE:
//
// 2 bytes = width
// 2 bytes = height
// 160x128 RGB565
//
// TOTAL = 40964
//
// We read source as 160x128 and rotate
// clockwise into 128x160.
//
// Destination:
//
// dstX = sourceY
// dstY = 127 - sourceX
// =====================================================

bool Media::showRotatedImage(File &file)
{
    if (!file)
    {
        return false;
    }

    if (file.size() != ROTATED_IMAGE_SIZE)
    {
        return false;
    }

    file.seek(0);

    // -------------------------------------------------
    // HEADER
    // -------------------------------------------------

    uint16_t sourceW =
        read16(file);

    uint16_t sourceH =
        read16(file);

    Serial.print("HEADER WIDTH: ");
    Serial.println(sourceW);

    Serial.print("HEADER HEIGHT: ");
    Serial.println(sourceH);

    // -------------------------------------------------
    // Validate header
    // -------------------------------------------------

    if (
        sourceW != 160 ||
        sourceH != 128
    )
    {
        Serial.println("INVALID ROTATED HEADER");
        return false;
    }

    // -------------------------------------------------
    // Buffer for one source line
    // -------------------------------------------------

    uint16_t sourceLine[160];

    // -------------------------------------------------
    // We cannot simply setRotation() here because
    // that would change the whole TFT orientation.
    //
    // Instead we physically remap each pixel.
    // -------------------------------------------------

    display.fill(ST77XX_BLACK);

    for (int sy = 0; sy < 128; sy++)
    {
        size_t bytesRead =
            file.read(
                (uint8_t*)sourceLine,
                160 * 2
            );

        if (bytesRead != 160 * 2)
        {
            Serial.println("ROTATED IMAGE READ ERROR");
            return false;
        }

        for (int sx = 0; sx < 160; sx++)
        {
            uint16_t color =
                sourceLine[sx];

            int dx =
                sy;

            int dy =
                127 - sx;

            // ---------------------------------------
            // After rotation:
            //
            // source 160x128
            // becomes
            // 128x160
            //
            // However, the destination coordinate
            // must stay inside 128x160.
            // ---------------------------------------

            if (
                dx >= 0 &&
                dx < 128 &&
                dy >= 0 &&
                dy < 160
            )
            {
                display.drawPixel(
                    dx,
                    dy,
                    color
                );
            }
        }
    }

    Serial.println("ROTATED IMAGE DISPLAYED");

    return true;
}

// =====================================================
// PLAY GIF
//
// GIF BIN FORMAT:
//
// frame1 = 40960 bytes
// frame2 = 40960 bytes
// frame3 = 40960 bytes
// ...
//
// IMPORTANT:
// GIF files are NOT interpreted using the 40964
// image-header rule.
//
// Every GIF frame remains 128x160.
// =====================================================

bool Media::playGIF(const char* path)
{
    if (!path)
    {
        return false;
    }

    closeFile();

    Serial.println();
    Serial.println("====================");
    Serial.println("MEDIA GIF");
    Serial.println("====================");

    Serial.print("PATH: ");
    Serial.println(path);

    // -------------------------------------------------
    // OPEN
    // -------------------------------------------------

    mediaFile =
        SD.open(
            path,
            FILE_READ
        );

    if (!mediaFile)
    {
        Serial.println("GIF OPEN FAILED");
        return false;
    }

    fileSize =
        mediaFile.size();

    Serial.print("GIF SIZE: ");
    Serial.println(fileSize);

    // -------------------------------------------------
    // GIF MUST BE MULTIPLE OF 40960
    // -------------------------------------------------

    if (
        fileSize < GIF_FRAME_SIZE ||
        (fileSize % GIF_FRAME_SIZE) != 0
    )
    {
        Serial.println("INVALID GIF BIN");

        mediaFile.close();

        return false;
    }

    // -------------------------------------------------
    // FRAME COUNT
    // -------------------------------------------------

    frameSize =
        GIF_FRAME_SIZE;

    frameCount =
        fileSize /
        frameSize;

    currentFrame = 0;

    frameDelay =
        GIF_FRAME_DELAY;

    lastFrameTime =
        0;

    isGif = true;
    playing = true;

    Serial.print("FRAME SIZE: ");
    Serial.println(frameSize);

    Serial.print("FRAME COUNT: ");
    Serial.println(frameCount);

    // -------------------------------------------------
    // DISPLAY FIRST FRAME IMMEDIATELY
    // -------------------------------------------------

    if (!loadNextFrame())
    {
        closeFile();
        return false;
    }

    lastFrameTime =
        millis();

    Serial.println("GIF PLAYING");

    return true;
}

// =====================================================
// LOAD NEXT GIF FRAME
// =====================================================

bool Media::loadNextFrame()
{
    if (!mediaFile)
    {
        return false;
    }

    if (currentFrame >= frameCount)
    {
        currentFrame = 0;
    }

    uint32_t offset =
        currentFrame *
        frameSize;

    if (!mediaFile.seek(offset))
    {
        Serial.println("GIF SEEK FAILED");
        return false;
    }

    drawFrame(mediaFile);

    currentFrame++;

    if (currentFrame >= frameCount)
    {
        currentFrame = 0;
    }

    return true;
}

// =====================================================
// DRAW GIF FRAME
//
// 128x160 RGB565
// 40960 bytes
// =====================================================

void Media::drawFrame(File &file)
{
    uint16_t line[IMAGE_W];

    for (int y = 0; y < IMAGE_H; y++)
    {
        size_t bytesRead =
            file.read(
                (uint8_t*)line,
                IMAGE_W * 2
            );

        if (bytesRead != IMAGE_W * 2)
        {
            Serial.println("GIF FRAME READ ERROR");
            return;
        }

        display.pushImage(
            0,
            y,
            IMAGE_W,
            1,
            line
        );
    }
}

// =====================================================
// UPDATE
// =====================================================

void Media::update()
{
    if (!playing)
    {
        return;
    }

    if (!isGif)
    {
        return;
    }

    unsigned long now =
        millis();

    if (
        now -
        lastFrameTime <
        frameDelay
    )
    {
        return;
    }

    lastFrameTime =
        now;

    loadNextFrame();
}
