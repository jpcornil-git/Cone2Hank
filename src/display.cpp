#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
//#include <Fonts/FreeMono9pt7b.h>
#include "globals.h"
#include "splash.h"
#include "display.h"
#include "menu.h"

unsigned long lastScreenUpdate=0;
unsigned long lastAliveUpdate=0;
unsigned long lastDisplayEvent=0;

bool blink = false;

// Screen saver date
int hCenter = 64;                      // horizontal center of animation
int vCenter = 32;                      // vertical center of animation
int Hstar, Vstar;                      // star location currently
int Quantity = 20;                     // number of stars
int StarProgress[20];                  // array that tracks progress of each star
int StarAngle[20] = {};                // array that tracks angle of each star
int interval[20] = {};                 // array used to track speed of each star

#define MENU_LINES 4
#define LINE_SIZE  24
#define FRAME_RATE 100 //ms
#define SCREENSAVER_DELAY 10000 //ms
#define BLINK_HALF_PERIOD 500 //ms

cDisplay::cDisplay(Adafruit_SH1106G *oled, cMenuItemBase *menu[]) {
	_oled = oled;
	_menu = menu;

	char lineBuffer[25];
	size_t blinkStart, blinkStop;
	for(_menuSize=0; _menu[_menuSize] != nullptr; _menuSize++) {
		_menu[_menuSize]->getText(lineBuffer, &blinkStart, &blinkStop);
	}

	_oled->clearDisplay();
	_oled->setTextSize(1);
	_oled->setTextColor(SH110X_WHITE);

	reset();
}

void cDisplay::reset() {
	_cursorCurrent = _cursorStart = 0;
	_state = NAVIGATION;

	for (int i = 0; i < Quantity; i++) {
		StarAngle[i] = random(360);
		interval[i] = (random(5)) * FRAME_RATE +1;
	}

	lastDisplayEvent = millis();
}

bool cDisplay::isScreenUpdate(bool bDisplayEvent) {
	unsigned long currentMillis = millis();

	if (bDisplayEvent) {
		lastDisplayEvent = millis();
	}

	if ((currentMillis - lastAliveUpdate) > BLINK_HALF_PERIOD) {
		blink = !blink;
		lastAliveUpdate = currentMillis;
	}

	if ((currentMillis - lastScreenUpdate) > FRAME_RATE) {
		lastScreenUpdate = currentMillis;
		return true;
	}
	return false;
}


void cDisplay::alive_indication() {
	if (blink) {
		_oled->drawRect(125, 1, 2, 2, SH110X_WHITE);
	}	
}


void cDisplay::screenSaver() {
	if (isScreenUpdate()) {	
		_oled->clearDisplay();

		for (int i = 0; i < Quantity; i++) {
			if (StarProgress[i] > 100) {
				StarProgress[i] = StarProgress[i] - 100;
				StarAngle[i] = random(360);
				interval[i] = (random(5))+1;
			}else{			
				StarProgress[i] = StarProgress[i] + interval[i];
			}
			
			if (StarProgress[i] > 0) {	
				Hstar = (hCenter + (sin(StarAngle[i] / 57.296) * StarProgress[i] ));
				Vstar = (vCenter + (cos(StarAngle[i] / 57.296) * StarProgress[i] ));
				_oled->drawPixel(Hstar, Vstar, SH110X_WHITE);
			}
		}
		_oled->display(); 
	}
}


void cDisplay::splashScreen() {
	_oled->drawBitmap(0,0,bouclette_bitmap_bouclette, 128, 64, 1);
	_oled->display();
}


void cDisplay::runScreen(const char *title, int counter) {
	if (isScreenUpdate(true)) {
		_oled->clearDisplay();
		_oled->drawFastHLine(0, 9                , SCREEN_WIDTH, 1);
		_oled->drawFastHLine(0, SCREEN_HEIGHT - 9, SCREEN_WIDTH, 1);
		_oled->setCursor(0,0);
		_oled->print(title);
		_oled->setCursor(15,22);
		_oled->setTextSize(3);
		_oled->println(counter);
		_oled->setTextSize(1);

		alive_indication();	
		_oled->display();
	}
}


void cDisplay::displayMenu(const char * title) {
	char lineBuffer[LINE_SIZE];
	size_t blinkStart, blinkStop;

	if (isScreenUpdate()) {
		_oled->clearDisplay();
		_oled->drawFastHLine(0, 9                , SCREEN_WIDTH, 1);
		_oled->drawFastHLine(0, SCREEN_HEIGHT - 9, SCREEN_WIDTH, 1);
		_oled->setCursor(0,0);
		_oled->print(title);

		for(size_t line=0; line < MENU_LINES; line++) {
			_oled->setCursor(5,11+line*9);
			_menu[_cursorStart+line]->getText(lineBuffer, &blinkStart, &blinkStop);
			_oled->print(lineBuffer);

			if(blinkStart != blinkStop) {
				if (blink) {
					_oled->setTextColor(SH110X_BLACK, SH110X_WHITE);
					lineBuffer[blinkStop]='\0';
					_oled->setCursor(5+blinkStart*6,11+line*9);
					_oled->print(lineBuffer+blinkStart);
					_oled->setTextColor(SH110X_WHITE, SH110X_BLACK);
				}
			}
		}

		_oled->drawRoundRect(0, 10 + (_cursorCurrent-_cursorStart) * 9, SCREEN_WIDTH, 9, 4, SH110X_WHITE);
		_oled->setCursor(0, SCREEN_HEIGHT - 7);
		_menu[_cursorCurrent]->getHelp(lineBuffer);
		_oled->print(lineBuffer);

		alive_indication();
		_oled->display();
	}
}

void cDisplay::configScreen(const char *title, cJoystick *joystick) {

	unsigned long currentMillis = millis();

	if (joystick->getEvent()) {
		lastDisplayEvent = currentMillis;
		uint8_t event = joystick->getState();
		cMenuItemBase::eMenuState itemState;
		switch (_state) {
			case EDIT:
				itemState = _menu[_cursorCurrent]->eventHandler(event);
				if ( itemState == cMenuItemBase::IDLE ) {
					_state = NAVIGATION;					
				}
				break;			
			case NAVIGATION:
			default:
				if (event == cNavigationEventMasks::UP) {
					_cursorCurrent = (_cursorCurrent > 0) ? _cursorCurrent-1 : 0;
					if ( _cursorStart > _cursorCurrent) {
						_cursorStart = _cursorCurrent;
					}
				} else if (event == cNavigationEventMasks::DOWN) {
					_cursorCurrent = (_cursorCurrent < (_menuSize-1)) ? _cursorCurrent+1 : _menuSize-1;
					if ( _cursorCurrent > (_cursorStart + MENU_LINES - 1) ) {
						_cursorStart = _cursorCurrent - (MENU_LINES - 1);
					}
				} else if (event == cNavigationEventMasks::SELECT) {
					_state = EDIT;
					_menu[_cursorCurrent]->select();
				}
				break;
		}
	}

	if ( (currentMillis - lastDisplayEvent) > SCREENSAVER_DELAY) {
		screenSaver();
	} else {
		displayMenu(title);
	}
}