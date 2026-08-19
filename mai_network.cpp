#include "mai_network.h"
#include <WiFi.h>
#include "config.h"

MaiNetwork network;


// ========================================================
// NETWORK BEGIN
// ========================================================

void MaiNetwork::begin()
{
    Serial.println();
    Serial.println("====================");
    Serial.println("NETWORK");
    Serial.println("====================");


    // ----------------------------------------------------
    // AP + STA
    // ----------------------------------------------------

    WiFi.mode(WIFI_AP_STA);


    // ----------------------------------------------------
    // MAI HOTSPOT
    // ----------------------------------------------------

    const char* AP_SSID = "MAI-Core";
    const char* AP_PASSWORD = "m-daisy";


    if(WiFi.softAP(AP_SSID, AP_PASSWORD))
    {
        Serial.println("HOTSPOT READY");

        Serial.print("AP SSID: ");
        Serial.println(AP_SSID);

        Serial.print("AP IP: ");
        Serial.println(WiFi.softAPIP());
    }
    else
    {
        Serial.println("HOTSPOT FAILED");
    }


    // ----------------------------------------------------
    // CONNECT TO NORMAL WIFI
    // ----------------------------------------------------

    Serial.println("Connecting...");


    WiFi.begin(
        WIFI_SSID,
        WIFI_PASSWORD
    );


    unsigned long startTime = millis();


    while(
        WiFi.status() != WL_CONNECTED &&
        millis() - startTime < 15000
    )
    {
        delay(500);

        Serial.print(".");
    }


    Serial.println();


    if(WiFi.status() == WL_CONNECTED)
    {
        Serial.println("WIFI CONNECTED");

        Serial.print("SSID: ");
        Serial.println(WiFi.SSID());

        Serial.print("IP: ");
        Serial.println(
            WiFi.localIP()
        );

        Serial.print("RSSI: ");
        Serial.print(
            WiFi.RSSI()
        );
        Serial.println(" dBm");
    }
    else
    {
        Serial.println("WIFI FAILED");

        Serial.println(
            "HOTSPOT MODE ONLY"
        );
    }


    Serial.println();
}


// ========================================================
// GET SSID
// ========================================================

String MaiNetwork::getSSID()
{
    if(WiFi.status() == WL_CONNECTED)
    {
        return WiFi.SSID();
    }

    return "Not Connected";
}


// ========================================================
// GET STA IP
// ========================================================

String MaiNetwork::getIP()
{
    if(WiFi.status() == WL_CONNECTED)
    {
        return WiFi.localIP().toString();
    }

    return "Not Connected";
}


// ========================================================
// GET RSSI
// ========================================================

int MaiNetwork::getRSSI()
{
    if(WiFi.status() == WL_CONNECTED)
    {
        return WiFi.RSSI();
    }

    return 0;
}