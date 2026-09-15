#include "mai_api.h"

#include "config.h"
#include "system.h"
#include "mai_network.h"
#include "media.h"
#include "storage.h"

#include <SD.h>

MaiAPI api;


// =====================================================
// CONSTRUCTOR
// =====================================================

MaiAPI::MaiAPI()
    : server(80)
{
}


// =====================================================
// CONTENT TYPE
// =====================================================

String MaiAPI::getContentType(
    const String& path
)
{
    if (path.endsWith(".html"))
        return "text/html";

    if (path.endsWith(".css"))
        return "text/css";

    if (path.endsWith(".js"))
        return "application/javascript";

    if (path.endsWith(".json"))
        return "application/json";

    if (path.endsWith(".png"))
        return "image/png";

    if (path.endsWith(".jpg"))
        return "image/jpeg";

    if (path.endsWith(".jpeg"))
        return "image/jpeg";

    if (path.endsWith(".gif"))
        return "image/gif";

    if (path.endsWith(".svg"))
        return "image/svg+xml";

    if (path.endsWith(".ico"))
        return "image/x-icon";

    if (path.endsWith(".txt"))
        return "text/plain";

    if (path.endsWith(".bin"))
        return "application/octet-stream";

    return "application/octet-stream";
}


// =====================================================
// STATIC FILES
// SD ONLY
// =====================================================

void MaiAPI::handleStaticFile(
    AsyncWebServerRequest* request
)
{
    String path =
        request->url();


    if (path == "/")
    {
        path = "/index.html";
    }


    // Security
    if (path.indexOf("..") >= 0)
    {
        request->send(
            403,
            "text/plain",
            "Forbidden"
        );

        return;
    }


    Serial.print("STATIC: ");
    Serial.println(path);


    if (
        storage.sdReady() &&
        SD.exists(path)
    )
    {
        String contentType =
            getContentType(path);


        AsyncWebServerResponse*
        response =
            request->beginResponse(
                SD,
                path,
                contentType
            );


        response->addHeader(
            "Cache-Control",
            "no-cache"
        );


        request->send(response);

        return;
    }


    Serial.println(
        "STATIC FILE NOT FOUND"
    );


    request->send(
        404,
        "text/plain",
        "File Not Found"
    );
}


// =====================================================
// RECURSIVE FILE LIST
// =====================================================

void MaiAPI::appendFilesRecursive(
    String& json,
    bool& first,
    const char* folder
)
{
    if (!storage.sdReady())
        return;


    File root =
        SD.open(folder);


    if (!root)
        return;


    if (!root.isDirectory())
    {
        root.close();
        return;
    }


    File file =
        root.openNextFile();


    while (file)
    {
        String name =
            file.name();


        // =================================================
        // DIRECTORY
        // =================================================

        if (file.isDirectory())
        {
            String subPath =
                name;

            file.close();

            appendFilesRecursive(
                json,
                first,
                subPath.c_str()
            );

            file =
                root.openNextFile();

            continue;
        }


        // =================================================
        // FILE
        // =================================================

        String cleanName =
            name.substring(
                name.lastIndexOf("/") + 1
            );


        String type =
            "other";


        if (
            name.startsWith(
                "/media/gifs/"
            )
        )
        {
            if (
                cleanName.endsWith(".bin")
            )
            {
                type = "gif";
            }
        }
        else if (
            name.startsWith(
                "/media/images/"
            )
        )
        {
            if (
                cleanName.endsWith(".bin")
            )
            {
                type = "image";
            }
        }
        else if (
            cleanName.endsWith(".html")
        )
        {
            type = "html";
        }
        else if (
            cleanName.endsWith(".css")
        )
        {
            type = "css";
        }
        else if (
            cleanName.endsWith(".js")
        )
        {
            type = "javascript";
        }
        else if (
            cleanName.endsWith(".json")
        )
        {
            type = "json";
        }


        if (!first)
            json += ",";

        first = false;


        json += "{";


        json += "\"name\":\"";
        json += cleanName;
        json += "\",";


        json += "\"path\":\"";
        json += name;
        json += "\",";


        json += "\"type\":\"";
        json += type;
        json += "\",";


        json += "\"size\":";
        json += String(
            (unsigned long long)
            file.size()
        );


        json += "}";


        file.close();


        file =
            root.openNextFile();
    }


    root.close();
}


// =====================================================
// BEGIN
// =====================================================

void MaiAPI::begin()
{
    Serial.println();
    Serial.println("====================");
    Serial.println("WEB API INIT");
    Serial.println("====================");


    // =================================================
    // MEDIA PLAY
    // =================================================

    server.on(
        "/api/media/play",
        HTTP_GET,
        [](AsyncWebServerRequest* request)
        {
            Serial.println(
                "PLAY ROUTE HIT"
            );


            if (
                !request->hasParam(
                    "file"
                )
            )
            {
                request->send(
                    400,
                    "application/json",
                    "{\"error\":\"missing file\"}"
                );

                return;
            }


            String file =
                request
                    ->getParam("file")
                    ->value();


            file.trim();


            Serial.print(
                "FILE: "
            );

            Serial.println(file);


            // Security
            if (
                file.indexOf("..") >= 0
            )
            {
                request->send(
                    403,
                    "application/json",
                    "{\"error\":\"forbidden\"}"
                );

                return;
            }


            // Media only
            if (
                !file.startsWith(
                    "/media/images/"
                )
                &&
                !file.startsWith(
                    "/media/gifs/"
                )
            )
            {
                request->send(
                    400,
                    "application/json",
                    "{\"error\":\"unsupported path\"}"
                );

                return;
            }


            if (
                !storage.exists(
                    file.c_str()
                )
            )
            {
                request->send(
                    404,
                    "application/json",
                    "{\"error\":\"file not found\"}"
                );

                return;
            }


            bool result =
                false;


            // GIF
            if (
                file.startsWith(
                    "/media/gifs/"
                )
            )
            {
                result =
                    media.playGIF(
                        file.c_str()
                    );


                request->send(
                    result ? 200 : 500,
                    "application/json",
                    result
                    ? "{\"status\":\"gif started\"}"
                    : "{\"error\":\"gif failed\"}"
                );

                return;
            }


            // IMAGE
            if (
                file.startsWith(
                    "/media/images/"
                )
            )
            {
                result =
                    media.showImage(
                        file.c_str()
                    );


                request->send(
                    result ? 200 : 500,
                    "application/json",
                    result
                    ? "{\"status\":\"image displayed\"}"
                    : "{\"error\":\"image failed\"}"
                );

                return;
            }
        }
    );


    // =================================================
    // SYSTEM
    // =================================================

    server.on(
        "/api/system",
        HTTP_GET,
        [](AsyncWebServerRequest* request)
        {
            String json = "{";


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
        }
    );


    // =================================================
    // NETWORK
    // =================================================

    server.on(
        "/api/network",
        HTTP_GET,
        [](AsyncWebServerRequest* request)
        {
            String json = "{";


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
        }
    );


    // =================================================
    // STORAGE
    // =================================================

    server.on(
        "/api/storage",
        HTTP_GET,
        [](AsyncWebServerRequest* request)
        {
            uint64_t total = 0;
            uint64_t used  = 0;


            if (
                storage.sdReady()
            )
            {
                total =
                    SD.totalBytes();

                used =
                    SD.usedBytes();
            }


            String json = "{";


            json += "\"ready\":";
            json +=
                storage.sdReady()
                ? "true"
                : "false";

            json += ",";


            json += "\"total\":";
            json += String(
                (unsigned long long)
                total
            );

            json += ",";


            json += "\"used\":";
            json += String(
                (unsigned long long)
                used
            );

            json += ",";


            json += "\"free\":";
            json += String(
                (unsigned long long)
                (total - used)
            );


            json += "}";


            request->send(
                200,
                "application/json",
                json
            );
        }
    );


    // =================================================
    // FILES
    // =================================================

    server.on(
        "/api/files",
        HTTP_GET,
        [](AsyncWebServerRequest* request)
        {
            if (
                !storage.sdReady()
            )
            {
                request->send(
                    500,
                    "application/json",
                    "{\"error\":\"SD unavailable\"}"
                );

                return;
            }


            String json =
                "[";


            bool first =
                true;


            api.appendFilesRecursive(
                json,
                first,
                "/"
            );


            json += "]";


            request->send(
                200,
                "application/json",
                json
            );
        }
    );


    // =================================================
    // STATIC WEB UI
    // =================================================

    server.onNotFound(
        [this](
            AsyncWebServerRequest*
            request
        )
        {
            handleStaticFile(request);
        }
    );


    // =================================================
    // START
    // =================================================

    server.begin();


    Serial.println(
        "API READY"
    );
}


// =====================================================
// HANDLE
// =====================================================

void MaiAPI::handle()
{
    // Async server
    // Nothing required.
}
