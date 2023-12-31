#ifndef JOYSTICK_H
#define JOYSTICK_H
#include <Arduino.h>

class cNavigationEventMasks {
	public:
		const static uint8_t IDLE   = 0b00000;
		const static uint8_t LEFT   = 0b00001;
		const static uint8_t RIGHT  = 0b00010;
		const static uint8_t UP     = 0b00100;
		const static uint8_t DOWN   = 0b01000;
		const static uint8_t SELECT = 0b10000;
};

class cJoystick {
    public:
        cJoystick(const uint8_t vx_pin, const uint8_t vy_pin, const uint8_t sw_pin);
        void reset();
        bool getEvent();
        uint8_t getState();
        void update();

    protected:
        uint8_t _vx_pin, _vy_pin, _sw_pin;
        uint8_t _currentState, _oldState, _sampledState;
        unsigned long _sampledTime;
};

#endif