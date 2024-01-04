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
    if (_state == IDLE) {  
        strcpy(help, _help);
    } else {
        strcpy(help, "Use \x18\x19\x1a\x1b then <enter>");
    }
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
// cMenuItemCB
// -----------------------------------------------------------------------------

cMenuItemCB::cMenuItemCB(const char *label, const char *help, const char *confirm, void (*callback)(void *), void *param) {
    _label = label;
    _help = help;
    _confirm = confirm;
    _callback = callback;
    _param = param;
    _lastSaveTime = 0;
}

void cMenuItemCB::getText(char *text, size_t *cursor_start, size_t *cursor_stop) {
    *cursor_start = *cursor_stop = 0;
    if (_confirm && (millis() - _lastSaveTime) < 1000) {
        sprintf(text, "%s (%s)", _label, _confirm);
    } else {
        strcpy(text, _label);
    }
}

cMenuItemBase::eMenuState cMenuItemCB::eventHandler(uint8_t event) {
    return IDLE;
}

void cMenuItemCB::getHelp(char *help) {  
    strcpy(help, _help);
}

void cMenuItemCB::select() {
    _callback(_param);
    _lastSaveTime = millis();
}

// -----------------------------------------------------------------------------
// cMenuItemFunc
// -----------------------------------------------------------------------------

cMenuItemUpDown::cMenuItemUpDown(const char *label, const char *help, void (*callback)(uint8_t event, void *), void *param) {
    _label = label;
    _help = help;
    _callback = callback;
    _param = param;
    _info = nullptr;
}

void cMenuItemUpDown::getText(char *text, size_t *cursor_start, size_t *cursor_stop) {
    if (_info) {
        sprintf(text, "%s %s", _label, _info);
        *cursor_stop = strlen(text);
        *cursor_start = *cursor_stop - strlen(_info);
    } else {
        strcpy(text, _label);
        *cursor_start = *cursor_stop = 0;
    }
}

cMenuItemBase::eMenuState cMenuItemUpDown::eventHandler(uint8_t event) {

    if (event & cNavigationEventMasks::SELECT) {
        _state = IDLE; // Exit edit mode
        _info = nullptr;
    } else {
        if (event & cNavigationEventMasks::UP) {
            _info = "Up";
        } else if (event & cNavigationEventMasks::DOWN) {
            _info = "Down";
        } else {
            _info = "Idle";
        }
        _callback(event, _param);
    }
    return _state;
}

void cMenuItemUpDown::getHelp(char *help) {    
    if (_state == IDLE) {
        strcpy(help, _help);
    } else {
        strcpy(help, "Use \x18\x19 then <enter>");
    };
}

void cMenuItemUpDown::select() {
    _state = EDIT;
}