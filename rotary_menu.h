#ifndef ROTARY_MENU_H
#define ROTARY_MENU_H

#include <Arduino.h>

class RotaryMenu
{
public:
    RotaryMenu();
    void begin();
    void update();

private:
    enum MenuState {
        MAIN_MENU,
        IMAGE_MENU,
        GIF_MENU,
        CLOCK_SCREEN,
        WAIT_SCREEN,
        MEDIA_SCREEN
    };
    MenuState state;
    MenuState previousMenuState;   // برای برگشت به منوی قبلی
    int previousSelected;          // فایل انتخاب‌شدهٔ قبلی

    static const uint8_t MAX_ITEMS = 24;
    static const uint8_t NAME_LEN  = 32;
    static const uint8_t PATH_LEN  = 96;

    char itemNames[MAX_ITEMS][NAME_LEN];
    char itemPaths[MAX_ITEMS][PATH_LEN];
    bool itemIsGIF[MAX_ITEMS];
    uint8_t itemCount;
    int selected;

    bool mediaPlaying;

    int lastCLK;
    bool lastSW;
    unsigned long lastButtonTime;
    unsigned long lastRotateTime;

    bool buttonPressed();
    void handleRotation();
    void handleButton();

    void scanDirectory(const char* folder, bool isGIF);
    void drawMainMenu();
    void drawFileMenu();
    void openImages();
    void openGIFs();
    void showSelected();

    void drawClock();
    void drawWait();

    void drawHeart(int x, int y, bool big);
    void drawTextSafe(const char* text, int x, int y, int size);
};

extern RotaryMenu rotaryMenu;
#endif
