#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_SH110X.h>
#include "menu.h"

class cDisplay {
    public:
        enum eDisplayState {NAVIGATION, EDIT};

        cDisplay(Adafruit_SH1106G *oled, cMenuItemBase *menu[]);
        void reset();
        void splashScreen();
        void configScreen(const char *title, cJoystick *joystick);
        void runScreen(const char *title, int counter);
    private:
        Adafruit_SH1106G *_oled;
        cMenuItemBase **_menu;
        eDisplayState _state;
        size_t _menuSize, _cursorCurrent, _cursorStart;

        bool isScreenUpdate(bool bDisplayEvent = false);
        void alive_indication();
        void screenSaver();
        void displayMenu(const char *title);
};

#endif