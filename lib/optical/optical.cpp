#include <util/atomic.h>
#include "optical.h"

cOptical *cOptical::_instance;

void cOptical::interruptHandler() {
    _instance->_counter++;
    _instance->_update = true;
}

cOptical::cOptical(uint8_t pin) : _pin(pin) {
    _instance = this;
    pinMode(_pin, INPUT);
    attachInterrupt(digitalPinToInterrupt(_pin), interruptHandler, RISING);
}

void cOptical::reset() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
       _counter = 0;
       _update = true;
    }
}

int cOptical::getCounter() {
    return _counter;
}

bool cOptical::update() {
    if ( _update ) {
        _update = false;
        return true;
    }
    return false;
}