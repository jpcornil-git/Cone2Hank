#ifndef GLOBALS_H
#define GLOBALS_H

#include <Adafruit_SH110X.h>
#include "motor.h"
#include "joystick.h"
#include "button.h"
#include "led.h"
#include "encoder.h"

#define SCREEN_I2C_ADDRESS 0x3c
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64

#define JOYSTICK_VX_PIN A0
#define JOYSTICK_VY_PIN A1
#define JOYSTICK_SW_PIN 4

#define BUTTON_SW_PIN   11
#define BUTTON_LED_PIN  12

#define MOTOR_PWN 6
#define MOTOR_IN1 5
#define MOTOR_IN2 7

#define ENCODER_IN 0

struct sHardware {
	cMotor 				motor;
	cJoystick 			joystick;
	cButton   			button;
	cLed				led_system, led_button;
	cEncoder  			encoder;
	Adafruit_SH1106G	oled;
};

#endif
