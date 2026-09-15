#ifndef MAI_WEB_H
#define MAI_WEB_H

#include <Arduino.h>
#include <WebServer.h>


class MaiWeb
{

private:

    WebServer server = WebServer(80);


public:

    void begin();

    void handle();


};


extern MaiWeb web;


#endif