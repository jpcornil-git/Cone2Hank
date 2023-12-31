#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>

class sSample {
    public:
        unsigned long   time;
        bool            state;
}; 

class cButton {
    public:
        enum eEvent {NONE, SHORT, LONG};

        cButton(const uint8_t pin);
        cButton(const uint8_t pin, const unsigned long minLongDuration);

        void reset();
        bool readPin();
        eEvent getEvent();
        void update();

    protected:
        uint8_t _pin;
        unsigned long _minLongDuration;
        sSample _last, _sampled, _posedge;
        eEvent  _event;
        bool    _eventTriggered;
};

#endif