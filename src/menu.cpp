#include <stdio.h>
#include "menu.h"
#include "joystick.h"

// -----------------------------------------------------------------------------
// cMenuItemInt
// -----------------------------------------------------------------------------

cMenuItemInt::cMenuItemInt(const char *label, const char *help, int *pIntVar, int min, int max) {
    _label = label;
    _help = help;
    _value = pIntVar;
    _min = min;
    _max = max;
 
    _ndigits= 1;
    for (int currentMax=9; currentMax < _max; currentMax=currentMax*10+9) {
        _ndigits++;
    }
    sprintf(_format, "%%s%%%dd", (int) _ndigits);

    reset();
}

void cMenuItemInt::reset() {
    _cursor = 0;
    _state = IDLE;
}

void cMenuItemInt::getText(char *text, size_t *cursor_start, size_t *cursor_stop) {
    int textLength = sprintf(text, _format, _label, *_value);
    *cursor_start = *cursor_stop = 0;
    if ( _state == EDIT ) {
        *cursor_stop = textLength - _cursor;
        *cursor_start = *cursor_stop - 1;
    }
    
}

void cMenuItemInt::getHelp(char *help) {    
    strcpy(help,_help);
}

cMenuItemBase::eMenuState cMenuItemInt::eventHandler(uint8_t event) {
    int tmpValue = *_value;
    int increment = 1;
    
    for (size_t i=0; i<_cursor; i++, increment*=10);

    switch (event) {
        case cNavigationEventMasks::UP :
            tmpValue += increment;
            if (tmpValue <= _max)
                *_value = tmpValue; 
            break;

        case cNavigationEventMasks::DOWN :
            tmpValue -= increment;
            if (tmpValue >= _min)
                *_value = tmpValue; 
            break;

        case cNavigationEventMasks::LEFT :
            if (_cursor < (_ndigits - 1))
                _cursor++;
            break;

        case cNavigationEventMasks::RIGHT :
            if (_cursor > 0)
                _cursor--;
            break;

        case cNavigationEventMasks::SELECT:
            _state = IDLE; // Exit edit mode
            break;
    }

    return _state;
}

void cMenuItemInt::select() {
    _cursor = 0;
    _state = EDIT;
}

// -----------------------------------------------------------------------------
// cMenuItemFunc
// -----------------------------------------------------------------------------

cMenuItemFunc::cMenuItemFunc(const char *label, const char *help, void (*callback)(void *), void *param) {
    _label = label;
    _help = help;
    _callback = callback;
    _param = param;
    _lastSaveTime = 0;
}

void cMenuItemFunc::getText(char *text, size_t *cursor_start, size_t *cursor_stop) {
    *cursor_start = *cursor_stop = 0;
    if ((millis() - _lastSaveTime) < 1000) {
        sprintf(text, "%s (Saved)", _label);
    } else {
        strcpy(text, _label);
    }
}

cMenuItemBase::eMenuState cMenuItemFunc::eventHandler(uint8_t event) {
    return IDLE;
}

void cMenuItemFunc::getHelp(char *help) {    
    strcpy(help, _help);
}

void cMenuItemFunc::select() {
    _callback(_param);
    _lastSaveTime = millis();
}