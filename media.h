#ifndef MEDIA_H
#define MEDIA_H

#include <Arduino.h>
#include <FS.h>
#include <SD.h>

class Media
{
public:

    Media();

    void begin();

    bool showImage(const char* path);

    bool playGIF(const char* path);

    void update();

    void stop();

    bool isPlaying();

private:

    File mediaFile;

    bool playing;
    bool isGif;

    uint32_t fileSize;
    uint32_t frameSize;
    uint32_t frameCount;
    uint32_t currentFrame;

    unsigned long lastFrameTime;
    unsigned long frameDelay;

    // -------------------------------------------------
    // IMAGE
    // -------------------------------------------------

    bool showNormalImage(File &file);

    bool showRotatedImage(File &file);

    // -------------------------------------------------
    // GIF
    // -------------------------------------------------

    bool loadNextFrame();

    void drawFrame(File &file);

    // -------------------------------------------------
    // HELPERS
    // -------------------------------------------------

    uint16_t read16(File &file);

    bool validImageSize(uint32_t size);

    void closeFile();
};

extern Media media;

#endif
