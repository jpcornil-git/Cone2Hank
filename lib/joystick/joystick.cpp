#include "joystick.h"

#define DEBOUNCE_TIME      10 // ms
#define REPEAT_TIME        500 // ms

#define VX_THRESHOLD_RIGHT 750
#define VX_THRESHOLD_LEFT  250

#define VY_THRESHOLD_UP    250
#define VY_THRESHOLD_DOWN  750

cJoystick::cJoystick(const uint8_t vx_pin, const uint8_t vy_pin, const uint8_t sw_pin) {
    _vx_pin = vx_pin;
    _vy_pin = vy_pin;
    _sw_pin = sw_pin;

    pinMode(_vx_pin, INPUT);
    pinMode(_vy_pin, INPUT);
    pinMode(_sw_pin, INPUT_PULLUP);
}

void cJoystick::reset() {
    _oldState = _currentState = _sampledState = cNavigationEventMasks::IDLE;
    _sampledTime = 0;

    update();
}

bool cJoystick::getEvent() {
    bool bEvent = false;
    
    if (_currentState != _oldState) {
        _oldState = _currentState;
        bEvent = true;
    } else {
        unsigned long currentTime = millis();
        if ((_currentState != 0) && ((currentTime - _sampledTime) > REPEAT_TIME )) {
            _sampledTime = currentTime;
            bEvent = true;        
        }
    }

    return bEvent;
}

uint8_t cJoystick::getState() {
    return _currentState;
}

void cJoystick::update() {
    int vx = analogRead(_vx_pin);
    int vy = analogRead(_vy_pin);
    bool sw = (bool) digitalRead(_sw_pin);

    uint8_t _newState = cNavigationEventMasks::IDLE;
    if (vx > VX_THRESHOLD_RIGHT)
        _newState = _newState | cNavigationEventMasks::RIGHT;
    else if (vx < VX_THRESHOLD_LEFT)
        _newState = _newState | cNavigationEventMasks::LEFT;
    if (vy > VY_THRESHOLD_DOWN)
        _newState = _newState | cNavigationEventMasks::DOWN;
    else if (vy < VY_THRESHOLD_UP)
        _newState = _newState | cNavigationEventMasks::UP;
    if (! sw)
        _newState = _newState | cNavigationEventMasks::SELECT;

    unsigned long currentTime = millis();
    if (_newState != _sampledState) {
        _sampledTime = currentTime;
        _sampledState = _newState;
    } else {
        if ((currentTime - _sampledTime) > DEBOUNCE_TIME) {
            _oldState = _currentState;
            _currentState = _sampledState;
        }
    }
}