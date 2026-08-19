#include "system.h"


SystemInfo systemInfo;



void SystemInfo::begin()
{
    startTime = millis();
}



uint32_t SystemInfo::cpuFreq()
{
    return ESP.getCpuFreqMHz();
}



uint32_t SystemInfo::freeRAM()
{
    return ESP.getFreeHeap();
}



uint32_t SystemInfo::flashSize()
{
    return ESP.getFlashChipSize() / 1024;
}



uint32_t SystemInfo::uptime()
{
    return (millis() - startTime) / 1000;
}