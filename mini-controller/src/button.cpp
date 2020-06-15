#include "Arduino.h"
#include "shared_macroses.h"

#include "button.h"

#define BUTTONSIG_ENABLE      ((_configFlags & BUTTONCFG_ENABLE_HIGH) == BUTTONCFG_ENABLE_HIGH ? HIGH : LOW)
#define BUTTONSIG_DISABLE     ((_configFlags & BUTTONCFG_ENABLE_HIGH) == BUTTONCFG_ENABLE_HIGH ? LOW : HIGH)
#define BUTTONSIG_INIT        ((_configFlags & BUTTONCFG_START_ON) == BUTTONCFG_START_ON ? BUTTONSIG_ENABLE : BUTTONSIG_DISABLE)


Button::Button(uint8_t buttonPin, uint8_t configFlags) :
    _buttonPin(buttonPin),
    _configFlags(configFlags),
    _stateFlags(BUTTONSTATE_EMPTY)
{
}

bool Button::begin(){
    bool result = true;
    if (!IS_FLAG_SET(BUTTONSTATE_ACTIVE, _stateFlags)){
        result = init();
        if (result) {
            SET_FLAG(BUTTONSTATE_ACTIVE, _stateFlags);
        }
    }
    return result;
}

bool Button::end(){
    bool result = true;
    if (!IS_FLAG_SET(BUTTONSTATE_ACTIVE, _stateFlags)){
        RESET_FLAG(BUTTONSTATE_ACTIVE, _stateFlags);
    }
    return result;
}

bool Button::getState(){
    return IS_FLAG_SET(BUTTONSTATE_ACTIVE, _stateFlags) 
        && digitalRead(_buttonPin) == BUTTONSIG_ENABLE;
}

bool Button::init(){
    pinMode(_buttonPin, INPUT);
    return true;
}