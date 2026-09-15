#include "rotary_menu.h"

#include <Arduino.h>
#include <SD.h>
#include <string.h>

#include "config.h"
#include "display.h"
#include "storage.h"
#include "media.h"


RotaryMenu rotaryMenu;


// =====================================================
// MENU CONFIG
// =====================================================

#define MENU_VISIBLE_ITEMS  6
#define MENU_ITEM_HEIGHT    19
#define MENU_START_Y        27

#define BUTTON_DEBOUNCE     250
#define ROTATE_DEBOUNCE     25


// =====================================================
// CONSTRUCTOR
// =====================================================

RotaryMenu::RotaryMenu()
{
    itemCount = 0;

    selected = 0;

    inMedia = false;

    lastCLK = HIGH;
    lastSW  = HIGH;

    lastButtonTime = 0;
    lastRotateTime = 0;


    for (uint8_t i = 0; i < MAX_ITEMS; i++)
    {
        itemNames[i][0] = '\0';
        itemPaths[i][0] = '\0';
        itemIsGIF[i] = false;
    }
}


// =====================================================
// BEGIN
// =====================================================

void RotaryMenu::begin()
{
    Serial.println();
    Serial.println("====================");
    Serial.println("ROTARY MENU");
    Serial.println("====================");


    pinMode(
        ROTARY_CLK,
        INPUT
    );

    pinMode(
        ROTARY_DT,
        INPUT
    );

    pinMode(
        ROTARY_SW,
        INPUT
    );


    lastCLK =
        digitalRead(
            ROTARY_CLK
        );

    lastSW =
        digitalRead(
            ROTARY_SW
        );


    scanFiles();


    Serial.print(
        "MENU ITEMS: "
    );

    Serial.println(
        itemCount
    );


    selected = 0;

    inMedia = false;


    drawMenu();
}


// =====================================================
// UPDATE
// =====================================================

void RotaryMenu::update()
{
    // =============================================
    // MEDIA MODE
    // =============================================

    if (inMedia)
    {
        /*
         * مهم:
         * اینجا media.update() نداریم.
         *
         * چون main loop خودش media.update()
         * را اجرا می‌کند.
         */

        if (buttonPressed())
        {
            Serial.println();
            Serial.println("====================");
            Serial.println("RETURN TO MENU");
            Serial.println("====================");


            media.stop();


            inMedia = false;


            delay(100);


            drawMenu();
        }

        return;
    }


    // =============================================
    // MENU MODE
    // =============================================

    handleRotation();

    handleButton();
}


// =====================================================
// BUTTON
// =====================================================

bool RotaryMenu::buttonPressed()
{
    int state =
        digitalRead(
            ROTARY_SW
        );


    unsigned long now =
        millis();


    bool pressed =
        (
            state == LOW &&
            lastSW == HIGH &&
            (
                now -
                lastButtonTime
            ) >=
            BUTTON_DEBOUNCE
        );


    lastSW = state;


    if (pressed)
    {
        lastButtonTime = now;

        return true;
    }


    return false;
}


// =====================================================
// ROTATION
// =====================================================

void RotaryMenu::handleRotation()
{
    if (itemCount == 0)
        return;


    int clk =
        digitalRead(
            ROTARY_CLK
        );


    if (
        clk != lastCLK &&
        clk == HIGH
    )
    {
        unsigned long now =
            millis();


        if (
            now -
            lastRotateTime >=
            ROTATE_DEBOUNCE
        )
        {
            int dt =
                digitalRead(
                    ROTARY_DT
                );


            if (dt != clk)
            {
                selected++;

                if (
                    selected >=
                    itemCount
                )
                {
                    selected = 0;
                }
            }
            else
            {
                selected--;

                if (selected < 0)
                {
                    selected =
                        itemCount - 1;
                }
            }


            lastRotateTime =
                now;


            drawMenu();
        }
    }


    lastCLK = clk;
}


// =====================================================
// BUTTON HANDLER
// =====================================================

void RotaryMenu::handleButton()
{
    if (!buttonPressed())
        return;


    if (itemCount == 0)
        return;


    Serial.println();
    Serial.println("====================");

    Serial.print(
        "SELECTED: "
    );

    Serial.println(
        itemNames[selected]
    );

    Serial.print(
        "PATH: "
    );

    Serial.println(
        itemPaths[selected]
    );

    Serial.println("====================");


    showSelected();
}


// =====================================================
// SHOW SELECTED
// =====================================================

void RotaryMenu::showSelected()
{
    if (selected < 0)
        return;


    if (
        selected >=
        itemCount
    )
    {
        return;
    }


    const char* path =
        itemPaths[selected];


    // =============================================
    // GIF
    // =============================================

    if (
        itemIsGIF[selected]
    )
    {
        Serial.println(
            "TYPE: GIF"
        );


        bool result =
            media.playGIF(
                path
            );


        if (result)
        {
            Serial.println(
                "GIF PLAYING"
            );

            inMedia = true;
        }
        else
        {
            Serial.println(
                "GIF FAILED"
            );

            delay(300);

            drawMenu();
        }


        return;
    }


    // =============================================
    // IMAGE
    // =============================================

    Serial.println(
        "TYPE: IMAGE"
    );


    bool result =
        media.showImage(
            path
        );


    if (result)
    {
        Serial.println(
            "IMAGE DISPLAYED"
        );

        inMedia = true;
    }
    else
    {
        Serial.println(
            "IMAGE FAILED"
        );

        delay(300);

        drawMenu();
    }
}


// =====================================================
// SCAN FILES
// =====================================================

void RotaryMenu::scanFiles()
{
    itemCount = 0;


    if (!storage.sdReady())
    {
        Serial.println(
            "SD NOT READY"
        );

        return;
    }


    // =============================================
    // IMAGES
    // =============================================

    scanDirectory(
        IMAGE_FOLDER,
        false
    );


    // =============================================
    // GIFS
    // =============================================

    scanDirectory(
        GIF_FOLDER,
        true
    );
}


// =====================================================
// SCAN DIRECTORY
// =====================================================

void RotaryMenu::scanDirectory(
    const char* folder,
    bool isGIF
)
{
    if (
        itemCount >=
        MAX_ITEMS
    )
    {
        return;
    }


    File dir =
        SD.open(
            folder
        );


    if (!dir)
    {
        Serial.print(
            "OPEN FAILED: "
        );

        Serial.println(
            folder
        );

        return;
    }


    if (!dir.isDirectory())
    {
        dir.close();

        return;
    }


    File file =
        dir.openNextFile();


    while (
        file &&
        itemCount < MAX_ITEMS
    )
    {
        if (!file.isDirectory())
        {
            const char* fullName =
                file.name();


            const char* slash =
                strrchr(
                    fullName,
                    '/'
                );


            const char* filename;


            if (slash)
            {
                filename =
                    slash + 1;
            }
            else
            {
                filename =
                    fullName;
            }


            size_t len =
                strlen(
                    filename
                );


            bool isBIN =
                (
                    len >= 4 &&
                    strcmp(
                        filename + len - 4,
                        ".bin"
                    ) == 0
                );


            if (isBIN)
            {
                // =================================
                // NAME
                // =================================

                strncpy(
                    itemNames[itemCount],
                    filename,
                    NAME_LEN - 1
                );


                itemNames[itemCount]
                    [NAME_LEN - 1] =
                    '\0';


                // =================================
                // FULL PATH
                // =================================

                snprintf(
                    itemPaths[itemCount],
                    PATH_LEN,
                    "%s/%s",
                    folder,
                    filename
                );


                // =================================
                // TYPE
                // =================================

                itemIsGIF[itemCount] =
                    isGIF;


                Serial.print(
                    "FOUND: "
                );

                Serial.print(
                    itemNames[itemCount]
                );

                Serial.print(
                    " -> "
                );

                Serial.println(
                    itemPaths[itemCount]
                );


                itemCount++;
            }
        }


        file.close();


        file =
            dir.openNextFile();
    }


    dir.close();
}


// =====================================================
// DRAW MENU
// =====================================================

void RotaryMenu::drawMenu()
{
    display.fill(
        ST77XX_BLACK
    );


    // =============================================
    // HEADER
    // =============================================

    display.drawText(
        "MAI MEDIA",
        4,
        2,
        2
    );


    display.drawLine(
        0,
        20,
        127,
        20,
        ST77XX_BLUE
    );


    // =============================================
    // NO FILES
    // =============================================

    if (itemCount == 0)
    {
        display.drawText(
            "NO FILES",
            25,
            70,
            2
        );

        return;
    }


    // =============================================
    // SCROLL WINDOW
    // =============================================

    int first =
        selected - 2;


    if (first < 0)
        first = 0;


    int maxFirst =
        itemCount -
        MENU_VISIBLE_ITEMS;


    if (maxFirst < 0)
        maxFirst = 0;


    if (first > maxFirst)
        first = maxFirst;


    // =============================================
    // ITEMS
    // =============================================

    for (
        uint8_t row = 0;
        row < MENU_VISIBLE_ITEMS;
        row++
    )
    {
        int index =
            first + row;


        if (
            index >=
            itemCount
        )
        {
            break;
        }


        int y =
            MENU_START_Y +
            row *
            MENU_ITEM_HEIGHT;


        // =========================================
        // SELECTED
        // =========================================

        if (
            index ==
            selected
        )
        {
            display.fillRect(
                0,
                y - 2,
                128,
                17,
                ST77XX_BLUE
            );


            display.drawText(
                ">",
                2,
                y,
                1
            );


            display.drawText(
                String(
                    itemNames[index]
                ),
                13,
                y,
                1
            );
        }

        // =========================================
        // NORMAL
        // =========================================

        else
        {
            display.drawText(
                String(
                    itemNames[index]
                ),
                13,
                y,
                1
            );
        }
    }


    // =============================================
    // FOOTER
    // =============================================

    display.drawLine(
        0,
        142,
        127,
        142,
        ST77XX_BLUE
    );


    display.drawText(
        "SW: OPEN",
        3,
        148,
        1
    );
}


// =====================================================
// SAFE TEXT
// =====================================================

void RotaryMenu::drawTextSafe(
    const char* text,
    int x,
    int y,
    int size
)
{
    if (!text)
        return;


    display.drawText(
        String(text),
        x,
        y,
        size
    );
}
