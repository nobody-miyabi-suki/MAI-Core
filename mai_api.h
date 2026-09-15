#ifndef MAI_API_H
#define MAI_API_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

class MaiAPI
{
private:

    AsyncWebServer server;

    String getContentType(
        const String& path
    );

    void handleStaticFile(
        AsyncWebServerRequest* request
    );

    void appendFilesRecursive(
        String& json,
        bool& first,
        const char* folder
    );


public:

    MaiAPI();

    void begin();

    void handle();
};

extern MaiAPI api;

#endif
