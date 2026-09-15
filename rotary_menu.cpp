#include "rotary_menu.h"
#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include "config.h"
#include "display.h"
#include "storage.h"
#include "media.h"

RotaryMenu rotaryMenu;

#define MENU_VISIBLE_ITEMS 6
#define MENU_ITEM_HEIGHT   18
#define MENU_START_Y       31
#define BUTTON_DEBOUNCE    250
#define ROTATE_DEBOUNCE    30

#define UI_BLACK       ST77XX_BLACK
#define UI_WHITE       ST77XX_WHITE
#define UI_PINK        0xF81F
#define UI_LIGHT_PINK  0xFDD7
#define UI_RED         0xF800
#define UI_DARK_RED    0x7800
#define UI_SOFT_RED    0xE104

// ---------- constructor ----------
RotaryMenu::RotaryMenu()
{
    state = MAIN_MENU;
    previousMenuState = MAIN_MENU;
    previousSelected = 0;
    itemCount = 0;
    selected = 0;
    lastCLK = HIGH;
    lastSW  = HIGH;
    lastButtonTime = 0;
    lastRotateTime = 0;
    mediaPlaying = false;

    for (uint8_t i = 0; i < MAX_ITEMS; i++) {
        itemNames[i][0] = '\0';
        itemPaths[i][0] = '\0';
        itemIsGIF[i] = false;
    }
}

// ---------- begin ----------
void RotaryMenu::begin()
{
    Serial.println();
    Serial.println("====================");
    Serial.println("ROTARY MENU");
    Serial.println("====================");

    pinMode(ROTARY_CLK, INPUT);
    pinMode(ROTARY_DT, INPUT);
    pinMode(ROTARY_SW, INPUT_PULLUP);

    lastCLK = digitalRead(ROTARY_CLK);
    lastSW  = digitalRead(ROTARY_SW);

    state = MAIN_MENU;
    selected = 0;
    drawMainMenu();
}

// ---------- update ----------
void RotaryMenu::update()
{
    if (state == MEDIA_SCREEN) {
        media.update();
        if (buttonPressed()) {
            media.stop();
            mediaPlaying = false;
            delay(150);
            // برگشت به منوی قبلی (Images یا GIFs) با حفظ انتخاب
            state = previousMenuState;
            selected = previousSelected;
            drawFileMenu();
        }
        return;
    }

    if (state == CLOCK_SCREEN) {
        drawClock();
        if (buttonPressed()) {
            state = MAIN_MENU;
            selected = 0;
            delay(150);
            drawMainMenu();
        }
        return;
    }

    if (state == WAIT_SCREEN) {
        drawWait();
        if (buttonPressed()) {
            state = MAIN_MENU;
            selected = 0;
            delay(150);
            drawMainMenu();
        }
        return;
    }

    handleRotation();
    handleButton();
}

// ---------- button & rotation ----------
bool RotaryMenu::buttonPressed()
{
    int stateNow = digitalRead(ROTARY_SW);
    unsigned long now = millis();
    bool pressed = (stateNow == LOW && lastSW == HIGH && (now - lastButtonTime) > BUTTON_DEBOUNCE);
    lastSW = stateNow;
    if (pressed) {
        lastButtonTime = now;
        return true;
    }
    return false;
}

void RotaryMenu::handleRotation()
{
    uint8_t maxItems = 0;
    if (state == MAIN_MENU) maxItems = 4;
    else maxItems = itemCount + 1;   // +1 برای BACK

    if (maxItems == 0) return;

    int clk = digitalRead(ROTARY_CLK);
    if (clk != lastCLK && clk == HIGH) {
        unsigned long now = millis();
        if (now - lastRotateTime > ROTATE_DEBOUNCE) {
            int dt = digitalRead(ROTARY_DT);
            if (dt != clk) {
                selected++;
                if (selected >= maxItems) selected = 0;
            } else {
                selected--;
                if (selected < 0) selected = maxItems - 1;
            }
            lastRotateTime = now;

            if (state == MAIN_MENU) drawMainMenu();
            else drawFileMenu();
        }
    }
    lastCLK = clk;
}

void RotaryMenu::handleButton()
{
    if (!buttonPressed()) return;

    if (state == MAIN_MENU) {
        switch (selected) {
            case 0: openImages(); break;
            case 1: openGIFs();  break;
            case 2: state = CLOCK_SCREEN; selected = 0; drawClock(); break;
            case 3: state = WAIT_SCREEN; selected = 0; drawWait(); break;
        }
        return;
    }

    if (state == IMAGE_MENU || state == GIF_MENU) {
        if (selected == itemCount) { // BACK
            state = MAIN_MENU;
            selected = 0;
            drawMainMenu();
        } else {
            // ذخیره‌ی منوی فعلی و گزینهٔ انتخاب‌شده برای برگشت بعدی
            previousMenuState = state;
            previousSelected  = selected;
            showSelected();
        }
    }
}

// ---------- file scanning & menus ----------
void RotaryMenu::scanDirectory(const char* folder, bool isGIF)
{
    if (itemCount >= MAX_ITEMS) return;
    File dir = SD.open(folder);
    if (!dir) return;
    if (!dir.isDirectory()) { dir.close(); return; }

    File file = dir.openNextFile();
    while (file && itemCount < MAX_ITEMS) {
        if (!file.isDirectory()) {
            const char* fullName = file.name();
            const char* slash = strrchr(fullName, '/');
            const char* filename = slash ? slash + 1 : fullName;
            size_t len = strlen(filename);
            if (len >= 4 && strcmp(filename + len - 4, ".bin") == 0) {
                strncpy(itemNames[itemCount], filename, NAME_LEN - 1);
                itemNames[itemCount][NAME_LEN - 1] = '\0';
                snprintf(itemPaths[itemCount], PATH_LEN, "%s/%s", folder, filename);
                itemIsGIF[itemCount] = isGIF;
                itemCount++;
            }
        }
        file.close();
        file = dir.openNextFile();
    }
    dir.close();
}

void RotaryMenu::openImages() {
    state = IMAGE_MENU; selected = 0; itemCount = 0;
    scanDirectory(IMAGE_FOLDER, false);
    drawFileMenu();
}
void RotaryMenu::openGIFs() {
    state = GIF_MENU; selected = 0; itemCount = 0;
    scanDirectory(GIF_FOLDER, true);
    drawFileMenu();
}

void RotaryMenu::drawMainMenu() {
    display.fill(UI_BLACK);
    display.drawLine(5,3,122,3,UI_PINK);
    drawHeart(10,11,false); drawHeart(117,11,true);
    display.drawText("MAI CORE", 32,8,2);
    display.drawLine(5,27,122,27,UI_RED);

    const char* names[4] = {"IMAGES", "GIFS", "CLOCK", "WAIT"};
    const char* icons[4] = {"<3", "*", "o", "~"};
    for (uint8_t i = 0; i < 4; i++) {
        int y = 39 + i * 23;
        if (selected == i) {
            display.fillRect(5, y-4, 118, 20, UI_DARK_RED);
            display.drawRect(5, y-4, 118, 20, UI_PINK);
            display.drawText(">", 10, y, 1);
            display.drawText(icons[i], 23, y, 1);
            display.drawText(names[i], 55, y, 1);
        } else {
            display.drawText(icons[i], 23, y, 1);
            display.drawText(names[i], 55, y, 1);
        }
    }
    display.drawLine(5,135,122,135,UI_PINK);
    display.drawText("SW = SELECT", 27, 143, 1);
}

void RotaryMenu::drawFileMenu() {
    display.fill(UI_BLACK);
    display.drawText(state == IMAGE_MENU ? "IMAGES" : "GIFS", state == IMAGE_MENU ? 39 : 48, 5, 2);
    display.drawLine(5, 25, 122, 25, UI_PINK);

    if (itemCount == 0) {
        display.drawText("NO FILES", 32, 65, 2);
        display.drawText("SW = BACK", 28, 105, 1);
        return;
    }

    uint8_t total = itemCount + 1;
    int first = selected - 2;
    if (first < 0) first = 0;
    if (first > total - MENU_VISIBLE_ITEMS) first = total - MENU_VISIBLE_ITEMS;
    if (first < 0) first = 0;

    for (uint8_t row = 0; row < MENU_VISIBLE_ITEMS; row++) {
        int index = first + row;
        if (index >= total) break;
        int y = MENU_START_Y + row * MENU_ITEM_HEIGHT;
        bool back = (index == itemCount);

        if (index == selected) {
            display.fillRect(2, y-2, 122, 17, UI_DARK_RED);
            display.drawRect(2, y-2, 122, 17, UI_PINK);
            display.drawText(">", 6, y, 1);
        }
        if (back) display.drawText("BACK", 22, y, 1);
        else {
            String name = String(itemNames[index]);
            if (name.length() > 17) name = name.substring(0, 17);
            display.drawText(name, 22, y, 1);
        }
    }

    if (total > MENU_VISIBLE_ITEMS) {
        int barHeight = 110 / total;
        if (barHeight < 6) barHeight = 6;
        int barY = 30 + (selected * (110 - barHeight)) / (total - 1);
        display.fillRect(124, barY, 3, barHeight, UI_PINK);
    }

    display.drawLine(3, 140, 124, 140, UI_RED);
    display.drawText("SW SELECT", 32, 146, 1);
}

// ---------- Clock & Wait screens ----------
void RotaryMenu::drawClock() {
    static unsigned long lastDraw = 0;
    unsigned long now = millis();
    if (now - lastDraw < 250) return;
    lastDraw = now;

    unsigned long totalSec = now / 1000;
    unsigned int h = totalSec / 3600, m = (totalSec % 3600) / 60, s = totalSec % 60;
    char buf[16];
    sprintf(buf, "%02u:%02u:%02u", h, m, s);

    display.fill(UI_BLACK);
    display.drawLine(5, 5, 122, 5, UI_PINK);
    drawHeart(12, 13, false); drawHeart(115, 13, true);
    display.drawText("CLOCK", 42, 13, 2);
    display.drawLine(5, 36, 122, 36, UI_RED);
    display.drawText(buf, 27, 65, 2);
    display.drawLine(20, 91, 107, 91, UI_PINK);
    display.drawText("UPTIME", 44, 103, 1);
    display.drawText("SW = BACK", 35, 140, 1);
}

void RotaryMenu::drawWait() {
    static unsigned long lastDraw = 0;
    static uint8_t frame = 0;
    unsigned long now = millis();
    if (now - lastDraw < 180) return;
    lastDraw = now;
    frame = (frame + 1) % 12;

    display.fill(UI_BLACK);
    display.drawLine(5, 5, 122, 5, UI_PINK);
    display.drawText("MAI CORE", 38, 14, 2);
    bool pulse = (frame == 0 || frame == 1 || frame == 10 || frame == 11);
    drawHeart(64, 65, pulse);
    const char* dots[4] = {".", "..", "...", "...."};
    uint8_t dotFrame = (frame / 3) % 4;
    display.drawText("PLEASE WAIT", 30, 95, 1);
    display.drawText(dots[dotFrame], 61, 108, 2);
    display.drawText("PROCESSING", 37, 123, 1);
    display.drawText("SW = BACK", 35, 143, 1);
}

// ---------- helpers ----------
void RotaryMenu::showSelected() {
    if (selected < 0 || selected >= itemCount) return;
    const char* path = itemPaths[selected];
    if (itemIsGIF[selected]) {
        if (media.playGIF(path)) {
            mediaPlaying = true;
            state = MEDIA_SCREEN;
        }
    } else {
        if (media.showImage(path)) {
            mediaPlaying = false;
            state = MEDIA_SCREEN;
        }
    }
}

void RotaryMenu::drawHeart(int x, int y, bool big) {
    if (big) {
        display.fillCircle(x-5, y-3, 6, UI_PINK);
        display.fillCircle(x+5, y-3, 6, UI_PINK);
        display.fillTriangle(x-11, y, x+11, y, x, y+13, UI_PINK);
        display.fillCircle(x, y+2, 2, UI_RED);
    } else {
        display.fillCircle(x-3, y-2, 3, UI_RED);
        display.fillCircle(x+3, y-2, 3, UI_RED);
        display.fillTriangle(x-6, y, x+6, y, x, y+8, UI_RED);
    }
}

void RotaryMenu::drawTextSafe(const char* text, int x, int y, int size) {
    if (text) display.drawText(String(text), x, y, size);
}
