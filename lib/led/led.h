#ifndef LED_H
#define LED_H
#include <Arduino.h>

class cLed {
  public:
    cLed(const uint8_t pin);
    cLed(const uint8_t pin, const uint8_t low, const uint8_t high);
    ~cLed() = default;

    void on();
    void off();
    void toggle();
    void blink(unsigned long timeOff, unsigned long timeOn);
    void flash(unsigned long timeOn);
    void update();
    // reset, blink with duration, flash with count

  private:
    enum class State {Off, On};
    enum class Mode {Solid, Blink, Flash};

    uint8_t        _pin;
    State          _state;
    Mode           _mode;
    uint8_t        _low, _high;

    unsigned long _timeOff, _timeOn;
    unsigned long _lasttime;

    void _write();
};

#endif

