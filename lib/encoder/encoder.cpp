#include <util/atomic.h>
#include "encoder.h"

#define N_COUNT_PER_TURN 16

#define NSAMPLES 8
#define LEVEL_LOW 2
#define LEVEL_HIGH 6

// _instance should be a list to support more than one encoder
// Constructor/destructor would then push/pop (in a critical section) with ISR looping thru it
cEncoder *_instance;
ISR(TIMER1_COMPA_vect)
{
    _instance->interruptHandler();
}

cEncoder::cEncoder(uint8_t pin) : _pin(pin) {
    _instance = this;
    pinMode(_pin, INPUT);

    // Setup Timer1 to sample pin value
    cli();
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) + (1 << CS12); // prescaler=256 and CTC mode
    OCR1A = 250; // 16MHz / 256 / 250 = 250Hz or 4ms
    TCNT1 = 0;
    TIMSK1 = (1 << OCIE1A);
    sei(); 
}

void cEncoder::reset(int initValue) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
       _counter = initValue * N_COUNT_PER_TURN;
       _shiftRegister = 0;
       _accumulator = 0;
       _level = LOW;
       _update = true;
    }
}

int cEncoder::getCounter() {
    return _counter/N_COUNT_PER_TURN;
}

bool cEncoder::update() {
    if ( _update ) {
        _update = false;
        return true;
    }
    return false;
}

void cEncoder::interruptHandler() {

    // Moving average over NSAMPLES
    if ( _shiftRegister & (1<<(NSAMPLES-1))) {
        _accumulator -= 1;
    }

    _shiftRegister <<= 1;
    if (digitalRead(_instance->_pin) == HIGH) {
        _accumulator += 1;
        _shiftRegister |= 1;
    }

    // Count down on rising edge (comparator with hysteresis)
    if (_level == LOW) {
        if (_accumulator > LEVEL_HIGH) {
            _level = HIGH;
            _counter--;
            _update = true;
        }
    } else {
        if (_accumulator < LEVEL_LOW) {
            _level = LOW;
        }
    }
}