#include "button.h"

#define DEBOUNCE_TIME      10 // ms
#define MIN_LONG_DURATION  1000 // ms

cButton::cButton(const uint8_t pin) : cButton(pin, MIN_LONG_DURATION) {}

cButton::cButton(const uint8_t pin, unsigned long minLongDuration) : _pin(pin), _minLongDuration(minLongDuration) {
    pinMode(_pin, INPUT_PULLUP);
}

bool cButton::readPin() {
    return ! (bool) digitalRead(_pin);
}

void cButton::reset() {
    _last = _sampled = _posedge = {.time=0, .state=readPin()};
    _event = NONE;
    _eventTriggered = true;
}

cButton::eEvent cButton::getEvent() {
    return _event;
}

void cButton::update() {
    unsigned long   currentTime = millis();
    bool            pinState = readPin();

    _event = NONE;

    if (pinState != _sampled.state) {
        _sampled = {.time=currentTime, .state=pinState};
    } else {
        // Debouncing
        if ( (currentTime - _sampled.time) > DEBOUNCE_TIME ) {
            // posedge
            if ( !_last.state & _sampled.state ) {
                _posedge = _sampled;
                _eventTriggered = false;
            // Active within SHORT period -> detect negedge for short event
            } else if ((currentTime - _posedge.time) < _minLongDuration) {
                if ( _last.state & !_sampled.state ) { 
                    _event = SHORT;
                }                
            // Active within LONG period -> long event then ignore
            } else if (_sampled.state && !_eventTriggered) {
                _event = LONG;
                _eventTriggered = true;
            }
            _last = _sampled;
        }
    }  
}