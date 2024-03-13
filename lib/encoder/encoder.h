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

  private:
    static void interruptHandler();
    static cEncoder *_instance;

    uint8_t          _pin;
    int              _counter;
    bool             _update;
};

#endif