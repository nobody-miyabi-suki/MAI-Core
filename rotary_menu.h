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

    // =========================
    // LIMITS
    // =========================

    static const uint8_t MAX_ITEMS = 24;
    static const uint8_t NAME_LEN  = 32;
    static const uint8_t PATH_LEN  = 64;


    // =========================
    // FILE DATA
    // =========================

    char itemNames[MAX_ITEMS][NAME_LEN];

    char itemPaths[MAX_ITEMS][PATH_LEN];

    bool itemIsGIF[MAX_ITEMS];


    // =========================
    // MENU STATE
    // =========================

    uint8_t itemCount;

    int selected;

    bool inMedia;


    // =========================
    // ROTARY STATE
    // =========================

    int lastCLK;

    bool lastSW;

    unsigned long lastButtonTime;

    unsigned long lastRotateTime;


    // =========================
    // FUNCTIONS
    // =========================

    bool buttonPressed();

    void handleRotation();

    void handleButton();

    void showSelected();

    void scanFiles();

    void scanDirectory(
        const char* folder,
        bool isGIF
    );

    void drawMenu();

    void drawTextSafe(
        const char* text,
        int x,
        int y,
        int size
    );
};


extern RotaryMenu rotaryMenu;

#endif
