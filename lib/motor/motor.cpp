#include "motor.h"

// L298N Motor Driver 
// en_pin in1_pin in2_pin
// 0      x       x         (Slow) Stop
// 1      0       0         Brake/Fast Stop
// 1      0       1         Backward
// 1      1       0         Forward
// 1      1       1         Brake (?)

cMotor::cMotor(uint8_t en_pin, uint8_t in1_pin, uint8_t in2_pin) {
    _en_pin  = en_pin;
    _in1_pin = in1_pin;
    _in2_pin = in2_pin;

    setState(STOP);

    pinMode(_in1_pin,OUTPUT);
    pinMode(_in2_pin,OUTPUT);
    pinMode(_en_pin,OUTPUT);
}

void cMotor::setSpeed(uint8_t speed) {
    analogWrite(_en_pin, speed);
}

cMotor::eState cMotor::getState(){
    return _state;
}

void cMotor::setState(eState state){
    _state = state;
    switch (_state) {
        case FORWARD:
            digitalWrite(_in1_pin, HIGH);
            digitalWrite(_in2_pin, LOW);
            break;
        case BACKWARD:
            digitalWrite(_in1_pin, LOW);
            digitalWrite(_in2_pin, HIGH);
            break;
        default: // STOP or BRAKE
            analogWrite(_en_pin, 0);
            digitalWrite(_in1_pin, LOW);
            digitalWrite(_in2_pin, LOW);
            if (_state == BRAKE)
                analogWrite(_en_pin, 255);
            break; 
    }
}