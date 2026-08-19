#include "mai_api.h"

#include "config.h"
#include "system.h"
#include "mai_network.h"
#include "media.h"

#include <SPIFFS.h>


MaiAPI api;



MaiAPI::MaiAPI()
:
server(80)
{

}



void MaiAPI::begin()
{


// =========================
// MEDIA PLAY
// =========================

server.on(
"/api/media/play",
HTTP_GET,
[](AsyncWebServerRequest *request)
{


Serial.println("PLAY ROUTE HIT");



if(!request->hasParam("file"))
{

    request->send(
        400,
        "application/json",
        "{\"error\":\"missing file\"}"
    );

    return;

}



String file =
request->getParam("file")->value();



file.trim();



Serial.print("FILE: ");
Serial.println(file);



if(!SPIFFS.exists(file))
{

    Serial.println("FILE NOT FOUND");


    request->send(
        404,
        "application/json",
        "{\"error\":\"file not found\"}"
    );


    return;

}



bool result=false;



// =====================
// GIF
// =====================

if(file.startsWith("/media/gifs/"))
{


Serial.println("TYPE GIF");


result =
media.playGIF(
file.c_str()
);



request->send(
result ? 200 : 500,
"application/json",
result ?
"{\"status\":\"gif started\"}" :
"{\"error\":\"gif failed\"}"
);



return;


}




// =====================
// IMAGE
// =====================


if(file.startsWith("/media/images/"))
{


Serial.println("TYPE IMAGE");


result =
media.showImage(
file.c_str()
);



request->send(
result ? 200 : 500,
"application/json",
result ?
"{\"status\":\"image displayed\"}" :
"{\"error\":\"image failed\"}"
);



return;


}




request->send(
400,
"application/json",
"{\"error\":\"unsupported path\"}"
);



});







// =========================
// SYSTEM API
// =========================

server.on(
"/api/system",
HTTP_GET,
[](AsyncWebServerRequest *request)
{

    String json="{";


    json += "\"name\":\"";
    json += MAI_NAME;
    json += "\",";


    json += "\"version\":\"";
    json += MAI_VERSION;
    json += "\",";


    json += "\"cpu\":";
    json += systemInfo.cpuFreq();
    json += ",";


    json += "\"ram\":";
    json += systemInfo.freeRAM();
    json += ",";


    json += "\"flash\":";
    json += systemInfo.flashSize();
    json += ",";


    json += "\"uptime\":";
    json += systemInfo.uptime();


    json += "}";


    request->send(
        200,
        "application/json",
        json
    );

});







// =========================
// NETWORK API
// =========================

server.on(
"/api/network",
HTTP_GET,
[](AsyncWebServerRequest *request)
{

    String json="{";


    json += "\"ssid\":\"";
    json += network.getSSID();
    json += "\",";


    json += "\"ip\":\"";
    json += network.getIP();
    json += "\",";


    json += "\"rssi\":";
    json += network.getRSSI();


    json += "}";


    request->send(
        200,
        "application/json",
        json
    );

});








// =========================
// STORAGE
// =========================

server.on(
"/api/storage",
HTTP_GET,
[](AsyncWebServerRequest *request)
{

    size_t total =
    SPIFFS.totalBytes();


    size_t used =
    SPIFFS.usedBytes();



    String json="{";


    json += "\"total\":";
    json += total;
    json += ",";


    json += "\"used\":";
    json += used;
    json += ",";


    json += "\"free\":";
    json += total-used;


    json += "}";


    request->send(
        200,
        "application/json",
        json
    );


});

// =========================
// FILE LIST (FIXED PATH)
// =========================

server.on(
"/api/files",
HTTP_GET,
[](AsyncWebServerRequest *request)
{

    File root = SPIFFS.open("/");

    File file = root.openNextFile();



    String json = "[";

    bool first = true;



    while(file)
    {


        String name = file.name();



        if(!first)
            json += ",";


        first = false;



        String type = "other";

        String path = name;



        // =====================
        // BIN MEDIA FILES
        // =====================

        if(name.endsWith(".bin"))
        {


            String cleanName =
            name.substring(
                name.lastIndexOf("/") + 1
            );



            // GIF

            if(
                cleanName == "delta.bin" ||
                cleanName == "aqua.bin" ||
                cleanName == "megumin.bin"
            )
            {

                type = "gif";


                path =
                "/media/gifs/" + cleanName;

            }


            // IMAGE

            else
            {

                type = "image";


                path =
                "/media/images/" + cleanName;

            }


        }



        json += "{";


        json += "\"name\":\"";

        json += name.substring(
            name.lastIndexOf("/") + 1
        );

        json += "\",";



        // مسیر قابل استفاده توسط سایت

        json += "\"path\":\"";

        json += path;

        json += "\",";



        json += "\"type\":\"";

        json += type;

        json += "\",";



        json += "\"size\":";

        json += file.size();



        json += "}";



        file =
        root.openNextFile();

    }



    json += "]";



    request->send(
        200,
        "application/json",
        json
    );


});



// =========================
// WEB UI
// =========================

server.serveStatic(
    "/",
    SPIFFS,
    "/"
)
.setDefaultFile(
    "index.html"
);



server.begin();


Serial.println(
    "API READY"
);

}
