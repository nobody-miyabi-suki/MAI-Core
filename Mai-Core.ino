#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <FS.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// ================= TFT =================
#define TFT_CS   5
#define TFT_RST  4
#define TFT_DC   2

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

// ================= WIFI =================
const char* wifi_ssid = "YOUR_WIFI";
const char* wifi_pass = "YOUR_PASSWORD";
const char* ap_ssid   = "MAI-Core"; 
const char* ap_pass   = "YOUR_PASSWORD";

AsyncWebServer server(80);

// ================= LED =================
#define LED_PIN 33
bool ledState = false;

// ================= MEDIA PLAYER =================
File mediaFile;
String currentMedia = "/media/gifs/cissia.bin";
bool playingGIF = true;
unsigned long lastFrame = 0;
const int frameDelay = 40;

// ================= OPEN MEDIA =================
bool openMedia(String path) {
  if (mediaFile) mediaFile.close();

  mediaFile = SPIFFS.open(path, "r");
  if (!mediaFile) {
    Serial.println("MEDIA ERROR");
    return false;
  }

  currentMedia = path;
  Serial.print("Playing: ");
  Serial.println(path);
  return true;
}

// ================= DRAW FRAME =================
void drawFrame() {
  uint16_t buffer[128];

  tft.startWrite();
  tft.setAddrWindow(0, 0, 128, 160);

  for (int y = 0; y < 160; y++) {
    mediaFile.read((uint8_t*)buffer, 256);
    tft.writePixels(buffer, 128, true);
  }

  tft.endWrite();
}

// ================= GIF PLAYER =================
void playGIF() {
  if (!mediaFile) return;
  if (millis() - lastFrame < frameDelay) return;

  lastFrame = millis();
  drawFrame();

  if (mediaFile.available() == 0) {
    mediaFile.seek(0);
  }
}

// ================= BOOT =================
void bootScreen() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(2);
  tft.setCursor(18, 25);
  tft.println("MAI CORE");

  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(20, 65);
  tft.println("Initializing...");
  tft.setCursor(20, 85);
  tft.println("ESP32     OK");
  tft.setCursor(20, 100);
  tft.println("TFT       OK");
  tft.setCursor(20, 115);
  tft.println("MEDIA     READY");

  delay(3000);
  tft.fillScreen(ST77XX_BLACK);
}

// ================= JSON =================
String systemJSON() {
  String json = "{";
  json += "\"cpu\":";   json += String(getCpuFrequencyMhz());
  json += ",";
  json += "\"ram\":";   json += String(ESP.getFreeHeap() / 1024);
  json += ",";
  json += "\"flash\":"; json += String(ESP.getFlashChipSize() / 1024);
  json += ",";
  json += "\"uptime\":"; json += String(millis() / 1000);
  json += "}";
  return json;
}

String networkJSON() {
  String json = "{";
  json += "\"ssid\":\""; json += WiFi.SSID();        json += "\",";
  json += "\"wifi\":\""; json += WiFi.localIP().toString(); json += "\",";
  json += "\"ap\":\"";   json += WiFi.softAPIP().toString(); json += "\"";
  json += "}";
  return json;
}


//testt
void drawImage(const char* path) {
  File file = SPIFFS.open(path, "r");

  if(!file){
    Serial.println("File not found");
    return;
  }

  uint16_t buffer[128];

  for(int y = 0; y < 160; y++) {

    file.read((uint8_t*)buffer, 128 * 2);

    tft.drawRGBBitmap(
      0,
      y,
      buffer,
      128,
      1
    );
  }

  file.close();
}

// ================= SERVER DECLARE =================
void startServer();

// ================= SETUP =================
void setup() {

  drawImage("/media/images/vermeil.bin");
  
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // TFT
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(0);
  tft.setSPISpeed(40000000);
  bootScreen();

  // SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS FAILED");
    return;
  }

  // WIFI
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ap_ssid, ap_pass);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  WiFi.begin(wifi_ssid, wifi_pass);

  // MEDIA
  openMedia("/media/gifs/cissia.bin");

  Serial.println("MAI CORE READY");
  startServer();
}

// ================= SERVER =================
void startServer() {
  // HOME
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/boot.html", "text/html");
  });

  // PANEL
  server.on("/panel", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // SYSTEM
  server.on("/api/system", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", systemJSON());
  });

  // NETWORK
  server.on("/api/network", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", networkJSON());
  });

  // PLAY MEDIA
  server.on("/api/play", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!request->hasParam("file")) {
      request->send(400, "text/plain", "No file");
      return;
    }

    String file = request->getParam("file")->value();
    if (!file.startsWith("/")) file = "/" + file;

    if (openMedia(file)) {
      if (file.indexOf("/gifs/") >= 0) playingGIF = true;
      else playingGIF = false;
      request->send(200, "text/plain", "Playing");
    } else {
      request->send(500, "text/plain", "Failed");
    }
  });

  // FILE LIST (only .bin files)
  server.on("/api/files", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = "[";
    File root = SPIFFS.open("/");
    if (!root) {
      request->send(200, "application/json", "[]");
      return;
    }

    File file = root.openNextFile();
    bool first = true;

    while (file) {
      String name = String(file.name());
      if (name.endsWith(".bin")) {
        if (!first) json += ",";
        json += "{";
        json += "\"name\":\"" + name + "\",";
        json += "\"size\":" + String(file.size());
        json += "}";
        first = false;
      }
      file = root.openNextFile();
    }

    json += "]";
    request->send(200, "application/json", json);
  });

  // LED
  server.on("/api/led", HTTP_GET, [](AsyncWebServerRequest *request) {
    ledState = !ledState;
    digitalWrite(LED_PIN, !ledState);
    String json = "{\"led\":" + String(ledState) + "}";
    request->send(200, "application/json", json);
  });

  // RESTART
  server.on("/api/restart", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Restarting");
    delay(500);
    ESP.restart();
  });

  // UPLOAD
  server.on("/api/upload", HTTP_POST,
    [](AsyncWebServerRequest *request) {
      request->send(200, "text/plain", "Uploaded");
    },
    [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
      static File upload;
      if (index == 0) {
        if (!filename.startsWith("/")) filename = "/" + filename;
        upload = SPIFFS.open(filename, "w");
      }
      if (upload) upload.write(data, len);
      if (final) {
        upload.close();
        Serial.println("UPLOAD DONE");
      }
    }
  );

  // DELETE
  server.on("/api/delete", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!request->hasParam("file")) {
      request->send(400, "text/plain", "No file");
      return;
    }
    String file = request->getParam("file")->value();
    if (SPIFFS.remove(file)) {
      request->send(200, "text/plain", "Deleted");
    } else {
      request->send(500, "text/plain", "Failed");
    }
  });

  // STATIC FILES
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.begin();
  Serial.println("SERVER STARTED");
}

// ================= LOOP =================
void loop() {
  if (playingGIF) playGIF();
}
