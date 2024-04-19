#include <Wire.h>
#include <EEPROM.h>
#include <avr/wdt.h>
#include "globals.h"
#include "menu.h"
#include "display.h"

// MCUSR saved in GPIOR0 by the (patched) bootloader
#define MCUSR_SAVED GPIOR0

enum eState {RESET, INIT, CONFIG, EDIT, RUNNING, PAUSED};
// systemState stored in the .noinit section as it is used by the WatchDog Reset (WDR) recovery
eState systemState __attribute__((section(".noinit")));

struct sHardware hardware;

struct sConfig {
	int magic = 0x3918;
	int numTurns;
	int maxSpeed;
} config;

cMenuItemBase **rootMenuItems;
cDisplay *display;

/* ---------------------------------------------------------------------------
	Utilities/menu callbacks
----------------------------------------------------------------------------*/

void eeprom_load(sConfig *config) {
	sConfig data;

	// Default Configuration 
	config->numTurns = 200;
	config->maxSpeed = 100;

	// Read settings from eeprom if any
	EEPROM.get(0, data);
	if (data.magic == config->magic) {
		*config = data;
	}
}


void eeprom_save(void *config) {
	EEPROM.put(0, *((sConfig *) config));
}


void move_motor(uint8_t event, void *param) {
	int speed = *((int *) param) / 4; 

	if (event & cNavigationEventMasks::UP) {
		hardware.motor->setSpeed((uint8_t) speed);
		hardware.motor->setState(cMotor::FORWARD);
	} else if (event & cNavigationEventMasks::DOWN) {
		hardware.motor->setSpeed((uint8_t) speed);
		hardware.motor->setState(cMotor::BACKWARD);		
	} else {
		hardware.motor->setState(cMotor::STOP);		
	} 
}


void cpu_reset(void *) {
	systemState = RESET;
}

/* ---------------------------------------------------------------------------
	Arduino setup/loop main section
----------------------------------------------------------------------------*/

void setup() {
	// Create hardware instances
	hardware.motor      = new cMotor(MOTOR_PWN, MOTOR_IN1, MOTOR_IN2);
	hardware.joystick   = new cJoystick(JOYSTICK_VX_PIN, JOYSTICK_VY_PIN, JOYSTICK_SW_PIN);
	hardware.button     = new cButton(BUTTON_SW_PIN);
	hardware.led_system = new cLed(LED_BUILTIN);
	hardware.led_button = new cLed(BUTTON_LED_PIN);
	hardware.encoder    = new cEncoder(ENCODER_IN);
	hardware.oled       = new Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

	// Alive indication
	hardware.led_system->blink(500,500);

	// Restore settings
	eeprom_load(&config);

	// Create OLED Menu
	rootMenuItems = new cMenuItemBase* [8] {
		(cMenuItemBase*) new cMenuItemInt(   "Turns     = ", "Number of turns", &config.numTurns, 0, 999),
		(cMenuItemBase*) new cMenuItemInt(   "Speed (%) = ", "Max. motor speed", &config.maxSpeed, 1, 100),
		(cMenuItemBase*) new cMenuItemUpDown("Up/Down",      "Motor control", move_motor, &config.maxSpeed),
		(cMenuItemBase*) new cMenuItemCB(    "Save",         "Save configuration", "Saved", eeprom_save, &config),
		(cMenuItemBase*) new cMenuItemCB(    "Reset",        "Reset CPU", nullptr, cpu_reset, nullptr),
		(cMenuItemBase*) nullptr
	};

	// wait for OLED to power up
	delay(250);

	// Initialize display
	hardware.oled->begin(SCREEN_I2C_ADDRESS, false);
	display = new cDisplay(hardware.oled, rootMenuItems);
	display->splashScreen();
	delay(1000);

	// Disable WDR recovery when system was not running
	if ((MCUSR_SAVED & (1<<WDRF)) && (systemState != RUNNING)) {
		MCUSR_SAVED = 0;
	}
	systemState = INIT;

	// Enable WatchDog
	wdt_enable(WDTO_1S);
}


void loop() {
	// Watchdog reset
	if (systemState != RESET) {
		wdt_reset();
	}

	// Update object states
	hardware.joystick->update();
	hardware.button->update();
	hardware.encoder->update();
	hardware.led_system->update();
	hardware.led_button->update();

	cButton::eEvent buttonEvent = hardware.button->getEvent();

	switch (systemState) {
		case CONFIG:
			if (buttonEvent == cButton::SHORT) {
				hardware.motor->setSpeed((uint8_t)config.maxSpeed);
				hardware.motor->setState(cMotor::FORWARD);
				hardware.encoder->reset(config.numTurns);
				systemState = RUNNING;
				hardware.led_button->on();
			}
			display->configScreen("Config", hardware.joystick);
			break;

		case RUNNING:
			if (buttonEvent == cButton::SHORT) {
				hardware.motor->setState(cMotor::STOP);
				systemState = PAUSED;
				hardware.led_button->blink(500,500);
			} else {
				if (hardware.encoder->getCounter() <= 0) {
					systemState = INIT;
				}
			}
			display->runScreen("Running", hardware.encoder);
			break;

		case PAUSED:
			if (buttonEvent == cButton::SHORT) {
				hardware.motor->setSpeed(config.maxSpeed);
				hardware.motor->setState(cMotor::FORWARD);
				systemState = RUNNING;
				hardware.led_button->on();
			}
			display->runScreen("Paused", hardware.encoder);
			break;

		case INIT:
			hardware.motor->setState(cMotor::STOP);
			hardware.joystick->reset();
			hardware.button->reset();
			// WDR handler/recovery
			// Only work if:
			// - bootloader saved a copy of MCUSR in MCUSR_SAVED
			// - encoder data are preserved after WDR (e.g. not within bss section but heap)
			if (MCUSR_SAVED & (1<<WDRF)) {
				MCUSR_SAVED = 0x0; // Reset WD indication
				hardware.led_button->blink(100,100);
				systemState = PAUSED;
			} else {
				hardware.led_button->off();
				hardware.encoder->reset();
				systemState = CONFIG;
			}
			break;

		default: // RESET (wait for WDR)
			break;	
	}

	if (buttonEvent == cButton::LONG) {
		systemState = INIT;
	}
}
