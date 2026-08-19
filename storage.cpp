#include "storage.h"


Storage storage;



bool Storage::begin()
{

    Serial.println(
        "Storage Ready"
    );


    return true;

}



bool Storage::exists(
    const char* path
)
{

    return SPIFFS.exists(path);

}



size_t Storage::fileSize(
    const char* path
)
{

    File file = SPIFFS.open(
        path,
        "r"
    );


    if(!file)
        return 0;


    size_t size = file.size();


    file.close();


    return size;

}



bool Storage::remove(
    const char* path
)
{

    return SPIFFS.remove(path);

}



void Storage::listFiles(
    const char* folder
)
{

    File root = SPIFFS.open(
        folder
    );


    if(!root)
    {

        Serial.println(
            "Folder not found"
        );

        return;

    }



    File file = root.openNextFile();



    while(file)
    {

        Serial.print(
            "FILE: "
        );


        Serial.print(
            file.name()
        );


        Serial.print(
            " SIZE: "
        );


        Serial.println(
            file.size()
        );


        file = root.openNextFile();

    }

}