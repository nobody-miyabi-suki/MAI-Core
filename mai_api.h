#ifndef MAI_API_H
#define MAI_API_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>


class MaiAPI
{

private:

    AsyncWebServer server;


public:

    MaiAPI();

    void begin();

    void handle();

};


extern MaiAPI api;


#endif