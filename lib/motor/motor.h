#include <Arduino.h>

class cMotor {
    public:
        enum eState {STOP, FORWARD, BACKWARD, BRAKE};

        cMotor(uint8_t en_pin, uint8_t in1_pin, uint8_t in2_pin);
        void setSpeed(uint8_t speed);
        eState getState();
        void setState(eState state);
    
    private:
        uint8_t _en_pin, _in1_pin, _in2_pin;
        eState _state;
};