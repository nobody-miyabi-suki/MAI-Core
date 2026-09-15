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

    // =====================================================
    // STATES
    // =====================================================

    enum MenuState
    {
        MAIN_MENU,
        IMAGE_MENU,
        GIF_MENU,
        CLOCK_SCREEN,
        WAIT_SCREEN,
        SYSTEM_SCREEN,
        MEDIA_SCREEN
    };

    MenuState state;

    // =====================================================
    // FILES
    // =====================================================

    static const uint8_t MAX_ITEMS = 24;
    static const uint8_t NAME_LEN  = 32;
    static const uint8_t PATH_LEN  = 96;

    char itemNames[MAX_ITEMS][NAME_LEN];
    char itemPaths[MAX_ITEMS][PATH_LEN];

    bool itemIsGIF[MAX_ITEMS];

    uint8_t itemCount;
    int selected;

    // =====================================================
    // MEDIA
    // =====================================================

    bool mediaPlaying;

    // =====================================================
    // ROTARY
    // =====================================================

    int lastCLK;
    bool lastSW;

    unsigned long lastButtonTime;
    unsigned long lastRotateTime;

    // =====================================================
    // INPUT
    // =====================================================

    bool buttonPressed();

    void handleRotation();
    void handleButton();

    // =====================================================
    // FILE SYSTEM
    // =====================================================

    void scanDirectory(
        const char* folder,
        bool isGIF
    );

    // =====================================================
    // MENU
    // =====================================================

    void drawMainMenu();

    void drawFileMenu();

    void openImages();
    void openGIFs();

    void showSelected();

    // =====================================================
    // SCREENS
    // =====================================================

    void drawClock();
    void drawWait();
    void drawSystem();

    // =====================================================
    // DRAW HELPERS
    // =====================================================

    void drawTextSafe(
        const char* text,
        int x,
        int y,
        int size
    );

    void drawHeart(
        int x,
        int y,
        bool big
    );

    void drawSparkle(
        int x,
        int y
    );

    void drawAnimeHeader(
        const char* title
    );
};

extern RotaryMenu rotaryMenu;

#endif
