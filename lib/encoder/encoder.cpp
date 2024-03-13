#include <util/atomic.h>
#include "encoder.h"

cEncoder *cEncoder::_instance;

void cEncoder::interruptHandler() {
    _instance->_counter--;
    _instance->_update = true;
}

cEncoder::cEncoder(uint8_t pin) : _pin(pin) {
    _instance = this;
    pinMode(_pin, INPUT);
    attachInterrupt(digitalPinToInterrupt(_pin), interruptHandler, RISING);
}

void cEncoder::reset(int initValue) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
       _counter = initValue;
       _update = true;
    }
}

int cEncoder::getCounter() {
    return _counter;
}

bool cEncoder::update() {
    if ( _update ) {
        _update = false;
        return true;
    }
    return false;
}