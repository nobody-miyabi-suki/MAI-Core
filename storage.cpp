#include "storage.h"

// =====================================================
// GLOBAL OBJECT
// =====================================================

Storage storage;


// =====================================================
// CONSTRUCTOR-LIKE INITIAL STATE
// =====================================================

bool Storage::begin()
{
    Serial.println();
    Serial.println("====================");
    Serial.println("STORAGE INIT");
    Serial.println("====================");


    _sdReady = false;


    // =================================================
    // SPI
    // =================================================

    Serial.println(
        "Initializing shared SPI..."
    );


    SPI.begin(
        SD_SCK,
        SD_MISO,
        SD_MOSI,
        SD_CS
    );


    delay(50);


    // =================================================
    // SD
    // =================================================

    Serial.println(
        "Initializing SD..."
    );


    if (
        !SD.begin(
            SD_CS,
            SPI,
            4000000
        )
    )
    {
        Serial.println(
            "SD FAILED!"
        );

        _sdReady = false;

        return false;
    }


    // =================================================
    // READY
    // =================================================

    _sdReady = true;


    Serial.println(
        "SD OK!"
    );


    // =================================================
    // CARD TYPE
    // =================================================

    uint8_t type =
        SD.cardType();


    Serial.print(
        "Card Type: "
    );


    if (type == CARD_NONE)
    {
        Serial.println(
            "NONE"
        );
    }
    else if (type == CARD_MMC)
    {
        Serial.println(
            "MMC"
        );
    }
    else if (type == CARD_SD)
    {
        Serial.println(
            "SDSC"
        );
    }
    else if (type == CARD_SDHC)
    {
        Serial.println(
            "SDHC"
        );
    }
    else
    {
        Serial.println(
            "UNKNOWN"
        );
    }


    // =================================================
    // CARD SIZE
    // =================================================

    uint64_t cardSize =
        SD.cardSize();


    Serial.print(
        "Card Size: "
    );


    Serial.print(
        cardSize /
        (1024ULL * 1024ULL)
    );


    Serial.println(
        " MB"
    );


    // =================================================
    // TOTAL
    // =================================================

    uint64_t total =
        SD.totalBytes();


    Serial.print(
        "Total: "
    );


    Serial.print(
        total
    );


    Serial.println(
        " bytes"
    );


    // =================================================
    // USED
    // =================================================

    uint64_t used =
        SD.usedBytes();


    Serial.print(
        "Used: "
    );


    Serial.print(
        used
    );


    Serial.println(
        " bytes"
    );


    // =================================================
    // FREE
    // =================================================

    Serial.print(
        "Free: "
    );


    Serial.print(
        total - used
    );


    Serial.println(
        " bytes"
    );


    // =================================================
    // READY
    // =================================================

    Serial.println();
    Serial.println("====================");
    Serial.println("STORAGE READY");
    Serial.println("====================");


    return true;
}


// =====================================================
// SD READY
// =====================================================

bool Storage::sdReady()
{
    return _sdReady;
}


// =====================================================
// EXISTS
// =====================================================

bool Storage::exists(
    const char* path
)
{
    if (!_sdReady)
        return false;


    if (path == nullptr)
        return false;


    return SD.exists(
        path
    );
}


// =====================================================
// FILE SIZE
// =====================================================

size_t Storage::fileSize(
    const char* path
)
{
    if (!_sdReady)
        return 0;


    if (path == nullptr)
        return 0;


    File file =
        SD.open(
            path,
            FILE_READ
        );


    if (!file)
        return 0;


    size_t size =
        file.size();


    file.close();


    return size;
}


// =====================================================
// REMOVE
// =====================================================

bool Storage::remove(
    const char* path
)
{
    if (!_sdReady)
        return false;


    if (path == nullptr)
        return false;


    if (!SD.exists(path))
        return false;


    return SD.remove(
        path
    );
}


// =====================================================
// LIST FILES
// =====================================================

void Storage::listFiles(
    const char* folder
)
{
    if (!_sdReady)
    {
        Serial.println(
            "SD NOT READY"
        );

        return;
    }


    Serial.println();
    Serial.println(
        "SD FILE LIST"
    );
    Serial.println(
        "------------"
    );


    listRecursive(
        folder,
        0
    );


    Serial.println(
        "------------"
    );
}


// =====================================================
// RECURSIVE LIST
// =====================================================

void Storage::listRecursive(
    const char* folder,
    uint8_t depth
)
{
    File root =
        SD.open(
            folder
        );


    if (!root)
    {
        Serial.print(
            "OPEN FAILED: "
        );

        Serial.println(
            folder
        );

        return;
    }


    if (!root.isDirectory())
    {
        root.close();

        return;
    }


    File file =
        root.openNextFile();


    while (file)
    {
        // =================================================
        // INDENT
        // =================================================

        for (
            uint8_t i = 0;
            i < depth;
            i++
        )
        {
            Serial.print(
                "  "
            );
        }


        // =================================================
        // DIRECTORY
        // =================================================

        if (
            file.isDirectory()
        )
        {
            Serial.print(
                "[DIR]  "
            );


            Serial.println(
                file.name()
            );


            String subPath =
                file.name();


            file.close();


            listRecursive(
                subPath.c_str(),
                depth + 1
            );
        }


        // =================================================
        // FILE
        // =================================================

        else
        {
            Serial.print(
                "[FILE] "
            );


            Serial.print(
                file.name()
            );


            Serial.print(
                "  "
            );


            Serial.print(
                file.size()
            );


            Serial.println(
                " bytes"
            );


            file.close();
        }


        // =================================================
        // NEXT
        // =================================================

        file =
            root.openNextFile();
    }


    root.close();
}
