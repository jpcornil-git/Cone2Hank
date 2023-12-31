#include "led.h"

cLed::cLed(const uint8_t pin) : cLed(pin, LOW, HIGH) {}

cLed::cLed(const uint8_t pin, const uint8_t low, const uint8_t high) : _pin(pin), _low(low), _high(high) {
  off();
  pinMode(_pin, OUTPUT);
}

void cLed::_write() {
  if( _state == State::Off)
    digitalWrite(_pin, _low);
  else
    digitalWrite(_pin, _high);

  _lasttime = millis();
}

void cLed::off() {
  _mode  = Mode::Solid;
  _state = State::Off;
  _write();
}

void cLed::on() {
  _mode  = Mode::Solid;
  _state = State::On;
  _write();
}

void cLed::toggle() {
  _state = (_state == State::On) ? State::Off : State::On;
  _write();
}

void cLed::blink(unsigned long timeOff, unsigned long timeOn) {
  _timeOff = timeOff;
  _timeOn = timeOn;

  _mode  = Mode::Blink;
  toggle();
}

void cLed::flash(unsigned long timeOn) {
  _timeOn = timeOn;

  _mode  = Mode::Flash;
  toggle();
}

void cLed::update() {
  unsigned long elapsedTime = millis() - _lasttime;

  switch (_mode) {
    case Mode::Blink :
      if ( _state == State::On ) {
        if ( elapsedTime >= _timeOn)
          toggle();
      } else {
        if ( elapsedTime >= _timeOff) {
          toggle();
        }
      }
      break;
    case Mode::Flash :
      if ( elapsedTime >= _timeOn) {
          _mode  = Mode::Solid;
          toggle();
      }
      break;
    default:
      break;
  }
}

