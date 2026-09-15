#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

// =====================================================
// SD PINS
// =====================================================

#define SD_CS   14
#define SD_SCK  25
#define SD_MISO 27
#define SD_MOSI 26


class Storage
{
public:

    // -----------------------------
    // Initialization
    // -----------------------------

    bool begin();

    // -----------------------------
    // Status
    // -----------------------------

    bool sdReady();

    // -----------------------------
    // File operations
    // -----------------------------

    bool exists(
        const char* path
    );

    size_t fileSize(
        const char* path
    );

    bool remove(
        const char* path
    );

    // -----------------------------
    // File listing
    // -----------------------------

    void listFiles(
        const char* folder
    );

private:

    // -----------------------------
    // SD status
    // -----------------------------

    bool _sdReady;

    // -----------------------------
    // Recursive listing
    // -----------------------------

    void listRecursive(
        const char* folder,
        uint8_t depth
    );
};


extern Storage storage;

#endif
