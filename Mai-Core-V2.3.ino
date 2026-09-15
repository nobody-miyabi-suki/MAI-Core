#include <Arduino.h>

#include "config.h"
#include "display.h"
#include "media.h"
#include "storage.h"
#include "mai_network.h"
#include "system.h"
#include "mai_api.h"
#include "rotary_menu.h"


void setup()
{
    Serial.begin(115200);
    delay(500);

    Serial.println();
    Serial.println("====================");
    Serial.println(MAI_NAME);
    Serial.println(MAI_VERSION);
    Serial.println("====================");


    // =========================
    // STORAGE
    // =========================

    storage.begin();


    // =========================
    // DISPLAY
    // =========================

    display.begin();


    // =========================
    // MEDIA
    // =========================

    media.begin();

    media.showImage(
        "/media/images/miyabi.bin"
    );

    Serial.println("Initial image displayed");


    // =========================
    // NETWORK
    // =========================

    network.begin();

    Serial.println();
    Serial.println("====================");
    Serial.println("NETWORK TEST");
    Serial.println("====================");

    Serial.print("SSID: ");
    Serial.println(network.getSSID());

    Serial.print("IP: ");
    Serial.println(network.getIP());

    Serial.print("RSSI: ");
    Serial.print(network.getRSSI());
    Serial.println(" dBm");


    // =========================
    // SYSTEM
    // =========================

    systemInfo.begin();

    Serial.println();
    Serial.println("====================");
    Serial.println("SYSTEM TEST");
    Serial.println("====================");

    Serial.print("CPU Frequency: ");
    Serial.print(systemInfo.cpuFreq());
    Serial.println(" MHz");

    Serial.print("Free RAM: ");
    Serial.print(systemInfo.freeRAM());
    Serial.println(" bytes");

    Serial.print("Flash: ");
    Serial.print(systemInfo.flashSize());
    Serial.println(" KB");

    Serial.print("Uptime: ");
    Serial.print(systemInfo.uptime());
    Serial.println(" sec");


    // =========================
    // ROTARY MENU
    // =========================

    rotaryMenu.begin();


    // =========================
    // WEB API
    // =========================

    api.begin();


    Serial.println();
    Serial.println("====================");
    Serial.println("SYSTEM READY");
    Serial.println("====================");
}


void loop()
{
    // Media فقط یک بار اینجا آپدیت می‌شود
    media.update();

    // Rotary فقط منو را کنترل می‌کند
    rotaryMenu.update();

    delay(2);
}
