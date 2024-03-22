#ifndef OPTICAL_H
#define OPTICAL_H
#include <Arduino.h>

class cEncoder {
  public:
    cEncoder(uint8_t pin);
    ~cEncoder() = default;

    void reset(int initValue = 0);
    int  getCounter();
    bool update();
    void interruptHandler();

  private:
    uint8_t          _pin;
    uint8_t          _shiftRegister;
    uint8_t          _accumulator;
    bool             _level;
    int              _counter;
    bool             _update;
};

#endif