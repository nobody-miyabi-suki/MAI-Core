#ifndef MAI_NETWORK_H
#define MAI_NETWORK_H

#include <Arduino.h>
#include <WiFi.h>


class MaiNetwork
{

public:

    void begin();

    String getSSID();

    String getIP();

    int getRSSI();

};


extern MaiNetwork network;

#endif