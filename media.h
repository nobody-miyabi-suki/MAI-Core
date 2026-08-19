#ifndef MEDIA_H
#define MEDIA_H

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>

class Media
{

public:

    void begin();

    bool showImage(const char* path);

    bool playGIF(const char* path);

    void update();


private:

    File gifFile;


    bool gifPlaying = false;


    unsigned long lastFrame = 0;


    uint16_t frameDelay = 100; 
    

    uint32_t frameCount = 0;

    uint32_t currentFrame = 0;



    void drawFrame();

};


extern Media media;


#endif
