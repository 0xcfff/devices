#include "Arduino.h"
#include "shared_macroses.h"
#include "tnlog.h"

#include "button.h"

#define BUTTONSIG_ENABLE      ((_configFlags & BUTTONCFG_ENABLE_HIGH) == BUTTONCFG_ENABLE_HIGH ? true : false)
#define BUTTONSIG_DISABLE     ((_configFlags & BUTTONCFG_ENABLE_HIGH) == BUTTONCFG_ENABLE_HIGH ? false : true)
#define BUTTONSIG_INIT        ((_configFlags & BUTTONCFG_START_ON) == BUTTONCFG_START_ON ? BUTTONSIG_ENABLE : BUTTONSIG_DISABLE)


Button::Button(DigitalPin * buttonPin, uint8_t configFlags, uint16_t bounceInterval, uint16_t doubleClickInterval, uint16_t longPressInterval) :
    _buttonPin(buttonPin),
    _configFlags(configFlags),
    _stateFlags(BUTTONSTATE_EMPTY),
    _lastChanged(0),
    _bounceInterval(bounceInterval),
    _doubleClickInterval(doubleClickInterval),
    _longPressInterval(longPressInterval)
{
}

bool Button::begin(){
    bool result = true;
    if (!IS_FLAG_SET(BUTTONSTATE_ACTIVE, _stateFlags)){
        if (IS_FLAG_SET(BUTTONCFG_ATTACHPININTERRUPT, _configFlags)) {
            std::function<void(DigitalPin*)> handler = std::bind(&Button::onInterrupt, this, std::placeholders::_1);
            _buttonPin->attachInterruptHandler(handler, CHANGE);
        }

        result = _buttonPin->begin();
        if (result) {
            SET_FLAG(BUTTONSTATE_ACTIVE, _stateFlags);
        }
    }
    return result;
}

bool Button::end(){
    bool result = true;
    if (!IS_FLAG_SET(BUTTONSTATE_ACTIVE, _stateFlags)){
        result = _buttonPin->end();
        if (result) {
            if (IS_FLAG_SET(BUTTONCFG_ATTACHPININTERRUPT, _configFlags)) {
                _buttonPin->detachInterruptHandler();
            }
            RESET_FLAG(BUTTONSTATE_ACTIVE, _stateFlags);
        }
    }
    return result;
}

bool Button::isPressed(){
    return IS_FLAG_SET(BUTTONSTATE_ACTIVE, _stateFlags) 
        && _buttonPin->digitalRead() == BUTTONSIG_ENABLE;
}

void Button::onInterrupt(DigitalPin * pin){
    LOG_INFOF("Button State: %i\n", (int)pin->digitalRead());
}