#ifndef SYSTEM_H
#define SYSTEM_H

#include <Arduino.h>


class SystemInfo
{

private:

    unsigned long startTime;


public:

    void begin();

    uint32_t cpuFreq();

    uint32_t freeRAM();

    uint32_t flashSize();

    uint32_t uptime();

};


extern SystemInfo systemInfo;


#endif