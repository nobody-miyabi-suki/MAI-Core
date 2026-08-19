#include <Arduino.h>
#include <SPIFFS.h>


// ===============================
// CONFIG
// ===============================

#include "config.h"


// ===============================
// DISPLAY
// ===============================

#include "display.h"


// ===============================
// MEDIA
// ===============================

#include "media.h"


// ===============================
// STORAGE
// ===============================

#include "storage.h"


// ===============================
// NETWORK
// ===============================

#include "mai_network.h"


// ===============================
// SYSTEM
// ===============================

#include "system.h"

// ===============================
// WEB
// ===============================
#include "mai_api.h"

// ===============================
// SETUP
// ===============================

void setup()
{

    Serial.begin(115200);

    delay(500);

    Serial.println();
    Serial.println("====================");
    Serial.println(MAI_NAME);
    Serial.println(MAI_VERSION);
    Serial.println("====================");

    Serial.print("Total: ");
Serial.println(SPIFFS.totalBytes());

Serial.print("Used: ");
Serial.println(SPIFFS.usedBytes());

    // ===========================
    // SPIFFS
    // ===========================

    if(!SPIFFS.begin(true))
    {

        Serial.println(
            "SPIFFS FAILED"
        );

        return;

    }


    Serial.println(
        "SPIFFS OK"
    );


    // ===========================
    // STORAGE
    // ===========================

    storage.begin();



    // ===========================
    // DISPLAY
    // ===========================

    display.begin();

display.bootAnimation(
    WiFi.SSID()
);


    // ===========================
    // MEDIA
    // ===========================

    media.begin();



    media.showImage(
        "/media/images/miyabi.bin"
    );



    Serial.println(
        "Image displayed"
    );




    // ===========================
    // SYSTEM
    // ===========================

    systemInfo.begin();



    Serial.println();
    Serial.println("====================");
    Serial.println("SYSTEM TEST");
    Serial.println("====================");



    Serial.print(
        "CPU Frequency: "
    );

    Serial.print(
        systemInfo.cpuFreq()
    );

    Serial.println(
        " MHz"
    );



    Serial.print(
        "Free RAM: "
    );

    Serial.print(
        systemInfo.freeRAM()
    );

    Serial.println(
        " bytes"
    );



    Serial.print(
        "Flash: "
    );

    Serial.print(
        systemInfo.flashSize()
    );

    Serial.println(
        " KB"
    );



    Serial.print(
        "Uptime: "
    );

    Serial.print(
        systemInfo.uptime()
    );

    Serial.println(
        " sec"
    );





// ===========================
// NETWORK
// ===========================

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

// ===============================
// WEB
// ===============================

api.begin();


Serial.println();

Serial.println(
    "SYSTEM READY"
);


}   // پایان setup



// ===============================
// LOOP
// ===============================

void loop()
{
media.update();
}
