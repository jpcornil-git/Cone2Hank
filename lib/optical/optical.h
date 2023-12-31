#ifndef OPTICAL_H
#define OPTICAL_H
#include <Arduino.h>

class cOptical {
  public:
    cOptical(uint8_t pin);
    ~cOptical() = default;

    void reset();
    int  getCounter();
    bool update();

  private:
    static void interruptHandler();
    static cOptical *_instance;

    uint8_t          _pin;
    int              _counter;
    bool             _update;
};

#endif