#include "mai_api.h"
#include "config.h"
#include "system.h"
#include "mai_network.h"
#include "media.h"
#include "storage.h"
#include <SD.h>

MaiAPI api;

MaiAPI::MaiAPI() : server(80) {}

String MaiAPI::getContentType(const String& path)
{
    if (path.endsWith(".html")) return "text/html";
    if (path.endsWith(".css"))  return "text/css";
    if (path.endsWith(".js"))   return "application/javascript";
    if (path.endsWith(".json")) return "application/json";
    if (path.endsWith(".png"))  return "image/png";
    if (path.endsWith(".jpg") || path.endsWith(".jpeg")) return "image/jpeg";
    if (path.endsWith(".gif"))  return "image/gif";
    if (path.endsWith(".svg"))  return "image/svg+xml";
    if (path.endsWith(".ico"))  return "image/x-icon";
    if (path.endsWith(".txt"))  return "text/plain";
    if (path.endsWith(".bin"))  return "application/octet-stream";
    return "application/octet-stream";
}

void MaiAPI::handleStaticFile(AsyncWebServerRequest* request)
{
    String path = request->url();
    if (path == "/") path = "/index.html";
    if (path.indexOf("..") >= 0) {
        request->send(403, "text/plain", "Forbidden");
        return;
    }
    Serial.print("STATIC: ");
    Serial.println(path);
    if (storage.sdReady() && SD.exists(path)) {
        String contentType = getContentType(path);
        AsyncWebServerResponse* response = request->beginResponse(SD, path, contentType);
        response->addHeader("Cache-Control", "no-cache");
        request->send(response);
        return;
    }
    Serial.println("STATIC FILE NOT FOUND");
    request->send(404, "text/plain", "File Not Found");
}

void MaiAPI::appendFilesRecursive(String& json, bool& first, const char* folder)
{
    if (!storage.sdReady()) return;
    File root = SD.open(folder);
    if (!root) return;
    if (!root.isDirectory()) { root.close(); return; }
    File file = root.openNextFile();
    while (file) {
        String name = file.name();
        if (file.isDirectory()) {
            String subPath = name;
            file.close();
            appendFilesRecursive(json, first, subPath.c_str());
            file = root.openNextFile();
            continue;
        }
        String cleanName = name.substring(name.lastIndexOf("/") + 1);
        String type = "other";
        if (name.startsWith("/media/gifs/")) {
            if (cleanName.endsWith(".bin")) type = "gif";
        } else if (name.startsWith("/media/images/")) {
            if (cleanName.endsWith(".bin")) type = "image";
        } else if (cleanName.endsWith(".html")) type = "html";
        else if (cleanName.endsWith(".css")) type = "css";
        else if (cleanName.endsWith(".js")) type = "javascript";
        else if (cleanName.endsWith(".json")) type = "json";
        if (!first) json += ",";
        first = false;
        json += "{";
        json += "\"name\":\"" + cleanName + "\",";
        json += "\"path\":\"" + name + "\",";
        json += "\"type\":\"" + type + "\",";
        json += "\"size\":" + String((unsigned long long)file.size());
        json += "}";
        file.close();
        file = root.openNextFile();
    }
    root.close();
}

void MaiAPI::begin()
{
    Serial.println();
    Serial.println("====================");
    Serial.println("WEB API INIT");
    Serial.println("====================");

    // ... (media play, system, network, storage, files endpoints – بدون تغییر) ...
    // =================================================
    // MEDIA PLAY
    // =================================================
    server.on("/api/media/play", HTTP_GET, [](AsyncWebServerRequest* request) {
        Serial.println("PLAY ROUTE HIT");
        if (!request->hasParam("file")) {
            request->send(400, "application/json", "{\"error\":\"missing file\"}");
            return;
        }
        String file = request->getParam("file")->value();
        file.trim();
        if (file.indexOf("..") >= 0) {
            request->send(403, "application/json", "{\"error\":\"forbidden\"}");
            return;
        }
        if (!file.startsWith("/media/images/") && !file.startsWith("/media/gifs/")) {
            request->send(400, "application/json", "{\"error\":\"unsupported path\"}");
            return;
        }
        if (!storage.exists(file.c_str())) {
            request->send(404, "application/json", "{\"error\":\"file not found\"}");
            return;
        }
        bool result = false;
        if (file.startsWith("/media/gifs/")) {
            result = media.playGIF(file.c_str());
            request->send(result ? 200 : 500, "application/json",
                         result ? "{\"status\":\"gif started\"}" : "{\"error\":\"gif failed\"}");
        } else {
            result = media.showImage(file.c_str());
            request->send(result ? 200 : 500, "application/json",
                         result ? "{\"status\":\"image displayed\"}" : "{\"error\":\"image failed\"}");
        }
    });

    server.on("/api/system", HTTP_GET, [](AsyncWebServerRequest* request) {
        String json = "{";
        json += "\"name\":\"" + String(MAI_NAME) + "\",";
        json += "\"version\":\"" + String(MAI_VERSION) + "\",";
        json += "\"cpu\":" + String(systemInfo.cpuFreq()) + ",";
        json += "\"ram\":" + String(systemInfo.freeRAM()) + ",";
        json += "\"flash\":" + String(systemInfo.flashSize()) + ",";
        json += "\"uptime\":" + String(systemInfo.uptime());
        json += "}";
        request->send(200, "application/json", json);
    });

    server.on("/api/network", HTTP_GET, [](AsyncWebServerRequest* request) {
        String json = "{";
        json += "\"ssid\":\"" + network.getSSID() + "\",";
        json += "\"ip\":\"" + network.getIP() + "\",";
        json += "\"rssi\":" + String(network.getRSSI());
        json += "}";
        request->send(200, "application/json", json);
    });

    server.on("/api/storage", HTTP_GET, [](AsyncWebServerRequest* request) {
        uint64_t total = 0, used = 0;
        if (storage.sdReady()) {
            total = SD.totalBytes();
            used = SD.usedBytes();
        }
        String json = "{";
        json += "\"ready\":" + String(storage.sdReady() ? "true" : "false") + ",";
        json += "\"total\":" + String((unsigned long long)total) + ",";
        json += "\"used\":" + String((unsigned long long)used) + ",";
        json += "\"free\":" + String((unsigned long long)(total - used));
        json += "}";
        request->send(200, "application/json", json);
    });

    server.on("/api/files", HTTP_GET, [](AsyncWebServerRequest* request) {
        if (!storage.sdReady()) {
            request->send(500, "application/json", "{\"error\":\"SD unavailable\"}");
            return;
        }
        String json = "[";
        bool first = true;
        api.appendFilesRecursive(json, first, "/");
        json += "]";
        request->send(200, "application/json", json);
    });

    // =====================================================
    // UPLOAD FORM PAGE (redirect)
    // =====================================================
    server.on("/upload", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->redirect("/index.html");
    });

    // =====================================================
    // FILE UPLOAD HANDLER (FULLY FIXED)
    // =====================================================
    server.on("/api/upload", HTTP_POST,
        // onRequest - خالی (همه‌ی پاسخ‌ها در upload handler داده می‌شوند)
        [](AsyncWebServerRequest* request) {},

        // upload handler
        [](AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final) {
            // چک امنیتی نام فایل
            if (filename.indexOf('/') >= 0 || filename.indexOf("..") >= 0) {
                if (!index) {
                    request->send(403, "application/json", "{\"error\":\"Invalid filename\"}");
                }
                return;
            }

            if (!index) {
                // ===== شروع فایل جدید =====
                // بررسی آماده بودن SD
                if (!storage.sdReady()) {
                    request->send(500, "application/json", "{\"error\":\"SD card not ready\"}");
                    return;
                }

                // دریافت پوشه از پارامتر POST
                String folder = "/media/images"; // پیش‌فرض
                if (request->hasParam("folder", true)) {
                    folder = request->getParam("folder", true)->value();
                }

                // فقط اجازه‌ی آپلود در /media/
                if (!folder.startsWith("/media/")) {
                    request->send(403, "application/json", "{\"error\":\"Forbidden path\"}");
                    return;
                }

                // ساخت پوشه در صورت عدم وجود
                if (!SD.exists(folder)) {
                    SD.mkdir(folder);
                }

                String fullPath = folder + "/" + filename;
                // اگر فایل وجود داشت، بازنویسی شود
                if (SD.exists(fullPath)) {
                    SD.remove(fullPath);
                }

                // باز کردن فایل برای نوشتن
                File f = SD.open(fullPath, FILE_WRITE);
                if (!f) {
                    Serial.printf("ERROR: Cannot create file %s\n", fullPath.c_str());
                    request->send(500, "application/json", "{\"error\":\"Cannot create file\"}");
                    return;
                }
                request->_tempFile = f; // ذخیره‌سازی شیء File
                Serial.printf("Upload started: %s\n", fullPath.c_str());
            }

            // نوشتن chunk (اگر فایل باز باشد)
            if (request->_tempFile) {
                request->_tempFile.write(data, len);
            } else {
                // فایل باز نشده – احتمالاً خطا در index==0 رخ داده و پاسخ ارسال شده
                if (final) {
                    // اطمینان از اینکه پاسخی ارسال نشده (فقط در صورت عدم ارسال قبلی)
                    // ولی چون _tempFile خالی است، خطا قبلاً مدیریت شده، پس کاری نمی‌کنیم.
                }
                return;
            }

            if (final) {
                // پایان فایل
                if (request->_tempFile) {
                    request->_tempFile.close();
                    request->_tempFile = File(); // خالی کردن
                    request->send(200, "application/json", "{\"status\":\"success\",\"message\":\"File uploaded\"}");
                    Serial.println("Upload completed.");
                } else {
                    // نباید به اینجا برسیم، اما اگر رسید پاسخ خطا بده
                    request->send(500, "application/json", "{\"error\":\"Unexpected error\"}");
                }
            }
        }
    );

    // =================================================
    // STATIC FILES (catch-all)
    // =================================================
    server.onNotFound([this](AsyncWebServerRequest* request) {
        handleStaticFile(request);
    });

    server.begin();
    Serial.println("API READY");
}

void MaiAPI::handle() {}
