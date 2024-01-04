#include <Wire.h>
#include <EEPROM.h>
#include "globals.h"
#include "menu.h"
#include "display.h"

#define N_COUNT_PER_TURN 16

enum eState {RESET, INIT, CONFIG, EDIT, RUNNING, PAUSED};
eState systemState;

struct sHardware hardware = {
	.motor      = cMotor(MOTOR_PWN, MOTOR_IN1, MOTOR_IN2),
	.joystick   = cJoystick(JOYSTICK_VX_PIN, JOYSTICK_VY_PIN, JOYSTICK_SW_PIN),
	.button 	= cButton(BUTTON_SW_PIN),
	.led_system = cLed(LED_BUILTIN),
	.led_button = cLed(BUTTON_LED_PIN),
	.optical 	= cOptical(OPTICAL_IN),
	.oled		= Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1)
};

struct sConfig {
	int magic = 0x3918;
	int numTurns;
	int maxSpeed;
} config;

cMenuItemBase **rootMenuItems;
cDisplay *display;

void (* CpuReset)(void *) = 0;

void eeprom_load(sConfig *config) {
	sConfig data;

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
		hardware.motor.setSpeed((uint8_t) speed);
		hardware.motor.setState(cMotor::FORWARD);
	} else if (event & cNavigationEventMasks::DOWN) {
		hardware.motor.setSpeed((uint8_t) speed);
		hardware.motor.setState(cMotor::BACKWARD);		
	} else {
		hardware.motor.setState(cMotor::BRAKE);		
	} 
}

void setup() {
	// Alive indication
	hardware.led_system.blink(500,500);

	// Default Configuration 
	// FIXME config from flash
	config.numTurns = 200;
	config.maxSpeed = 100;

	// Restore settings
	eeprom_load(&config);

	rootMenuItems = new cMenuItemBase* [8] {
		(cMenuItemBase*) new cMenuItemInt(   "Turns     = ", "Number of turns", &config.numTurns, 0, 999),
		(cMenuItemBase*) new cMenuItemInt(   "Speed (%) = ", "Max. motor speed", &config.maxSpeed, 1, 100),
		(cMenuItemBase*) new cMenuItemUpDown("Up/Down",      "Motor control", move_motor, &config.maxSpeed),
		(cMenuItemBase*) new cMenuItemCB(    "Save",         "Save configuration", "Saved", eeprom_save, &config),
		(cMenuItemBase*) new cMenuItemCB(    "Reset",        "Reset CPU", nullptr, CpuReset, nullptr),
		(cMenuItemBase*) nullptr
	};
	delay(250); // wait for OLED to power up
	hardware.oled.begin(SCREEN_I2C_ADDRESS, false);
	display = new cDisplay(&hardware.oled, rootMenuItems);

	display->splashScreen();
	delay(1000);

	systemState = INIT;
}

void loop() {
	hardware.joystick.update();
	hardware.button.update();
	hardware.optical.update();
	hardware.led_system.update();
	hardware.led_button.update();

	cButton::eEvent buttonEvent = hardware.button.getEvent();

	switch (systemState) {
		case CONFIG:
			if (buttonEvent == cButton::SHORT) {
				hardware.motor.setSpeed((uint8_t)config.maxSpeed);
				hardware.motor.setState(cMotor::FORWARD);
				hardware.optical.reset();
				systemState = RUNNING;
				hardware.led_button.on();
			}
			display->configScreen("Config", &hardware.joystick);
			break;

		case RUNNING:
			if (buttonEvent == cButton::SHORT) {
				hardware.motor.setState(cMotor::BRAKE);
				systemState = PAUSED;
				hardware.led_button.blink(500,500);
			} else {
				if (hardware.optical.getCounter() >= N_COUNT_PER_TURN*config.numTurns) {
					systemState = INIT;
				}
			}
			display->runScreen("Running", hardware.optical.getCounter()/N_COUNT_PER_TURN);
			break;

		case PAUSED:
			if (buttonEvent == cButton::SHORT) {
				hardware.motor.setSpeed(config.maxSpeed);
				hardware.motor.setState(cMotor::FORWARD);
				systemState = RUNNING;
				hardware.led_button.on();
			}
			display->runScreen("Paused", hardware.optical.getCounter()/N_COUNT_PER_TURN);
			break;

		case INIT:
			hardware.motor.setState(cMotor::BRAKE);
			hardware.joystick.reset();
			hardware.button.reset();
			hardware.led_button.off();
			hardware.optical.reset();

			systemState = CONFIG;
			break;

		default: // RESET
			CpuReset(nullptr);
			break;	
	}

	if (buttonEvent == cButton::LONG) {
		systemState = INIT;
	}
}
