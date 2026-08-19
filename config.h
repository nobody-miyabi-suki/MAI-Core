#ifndef CONFIG_H
#define CONFIG_H

// =====================
// MAI CORE CONFIG
// =====================

#define MAI_NAME    "MAI Core"
#define MAI_VERSION "2.0"


// =====================
// TFT CONFIG
// =====================

#define TFT_CS   13
#define TFT_DC   12
#define TFT_RST  33

#define TFT_WIDTH  128
#define TFT_HEIGHT 160


// =====================
// SD CONFIG
// =====================

#define SD_CS   14
#define SD_SCK  25
#define SD_MISO 27
#define SD_MOSI 26


// =====================
// ROTARY ENCODER
// =====================

#define ROT_CLK 32
#define ROT_DT  34
#define ROT_SW  5


// =====================
// FILE SYSTEM
// =====================

#define USE_SPIFFS true


// =====================
// WIFI
// =====================

#define WIFI_SSID     "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_PASS"


// =====================
// MEDIA PATHS
// =====================

#define MEDIA_FOLDER "/media"

#define IMAGE_FOLDER "/media/images"
#define GIF_FOLDER   "/media/gifs"


#endif
