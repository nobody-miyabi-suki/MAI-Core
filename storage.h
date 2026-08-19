#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include <SPIFFS.h>


class Storage
{

public:

    bool begin();

    bool exists(
        const char* path
    );


    size_t fileSize(
        const char* path
    );


    bool remove(
        const char* path
    );


    void listFiles(
        const char* folder
    );

};


extern Storage storage;


#endif