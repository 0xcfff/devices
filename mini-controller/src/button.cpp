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
    _lastChangedMillis(0),
    _lastClickMillis(0),
    _bounceInterval(bounceInterval),
    _doubleClickInterval(doubleClickInterval),
    _longPressInterval(longPressInterval)
{
}

bool Button::tick(){
    if (IS_FLAG_SET(BUTTONSTATE_ACTIVE, _stateFlags)) {
        refreshState(false);
        return true;
    }
    return false;
}

bool Button::isPressed(){
    return IS_FLAG_SET(BUTTONSTATE_ACTIVE, _stateFlags) 
        && _buttonPin->digitalRead() == BUTTONSIG_ENABLE;
}

bool Button::isReleased() {
    return IS_FLAG_SET(BUTTONSTATE_ACTIVE, _stateFlags) 
        && _buttonPin->digitalRead() == BUTTONSIG_DISABLE;
}

bool Button::isChanged(){
    return IS_FLAG_SET(BUTTONSTATE_ACTIVE, _stateFlags)
        && IS_FLAG_SET(BUTTONSTATE_HASCHANGED, _stateFlags);
}

bool Button::isActionPerformed(){
    return getButtonAction() != BUTTON_ACTION_NONE;
}

ButtonChange Button::getButtonChange(){
    if (!IS_FLAG_SET(BUTTONSTATE_ACTIVE, _stateFlags) || !IS_FLAG_SET(BUTTONSTATE_HASCHANGED, _stateFlags)){
        return BUTTON_CHANGE_NONE;
    }
    return IS_FLAG_SET(BUTTONSTATE_ISPRESSED, _stateFlags)
        ? BUTTON_CHANGE_PRESSED
        : BUTTON_CHANGE_RELEASED;
}

ButtonAction Button::getButtonAction(){
    if (!IS_FLAG_SET(BUTTONSTATE_ACTIVE, _stateFlags) || !IS_FLAG_SET(BUTTONSTATE_HASCHANGED, _stateFlags)) {
        return BUTTON_ACTION_NONE;
    }
    if (IS_FLAG_SET(BUTTONSTATE_ISLONGPRESSED, _stateFlags)) {
        return BUTTON_ACTION_LONGPRESS;
    }
    if (IS_FLAG_SET(BUTTONSTATE_ISCLICK, _stateFlags)) {
        return BUTTON_ACTION_CLICK;
    }
    if (IS_FLAG_SET(BUTTONSTATE_ISDBLCLICK, _stateFlags)) {
        return BUTTON_ACTION_DBLCLICK;
    }
    return BUTTON_ACTION_NONE;
}

void Button::refreshState(bool isInterrupt){
    if (!isInterrupt && IS_FLAG_SET(BUTTONSTATE_INTERRUPTHANDELED, _stateFlags)) {
        RESET_FLAG(BUTTONSTATE_INTERRUPTHANDELED, _stateFlags);
    } else if ((millis() - _lastChangedMillis) < _bounceInterval) {
        // do nothing
    } else {
        RESET_FLAG(BUTTONSTATE_HASCHANGED, _stateFlags);    
        RESET_FLAG(BUTTONSTATE_ISCLICK, _stateFlags);    
        RESET_FLAG(BUTTONSTATE_ISDBLCLICK, _stateFlags);    
        SET_FLAG_VALUE(BUTTONSTATE_INTERRUPTHANDELED, isInterrupt, _stateFlags);

        bool isPressed = _buttonPin->digitalRead() == BUTTONSIG_ENABLE;
        LOG_INFOF("isPressed(in): %i, diff: %i, lastChanged: %lu, millis: %lu, isInterrupt: %i <--\n", (int)isPressed, (int)(millis() - _lastChangedMillis), _lastChangedMillis, millis(), (int)isInterrupt);
        if (isPressed != IS_FLAG_SET(BUTTONSTATE_ISPRESSED, _stateFlags)){
            bool isChanged = true;
            // decode button release states
            if (!isPressed) {
                // actions triggered by button release action
                if (IS_FLAG_SET(BUTTONSTATE_ISLONGPRESSED, _stateFlags)) {
                    RESET_FLAG(BUTTONSTATE_ISLONGPRESSED, _stateFlags);
                    isChanged = false;
                } else if ((millis() - _lastClickMillis) < _doubleClickInterval) {
                    LOG_INFOLN("DblClick (in)");
                    SET_FLAG(BUTTONSTATE_ISDBLCLICK, _stateFlags);
                } else {
                    LOG_INFOLN("Click (in)");
                    SET_FLAG(BUTTONSTATE_ISCLICK, _stateFlags);
                    _lastClickMillis = millis();
                }
                
            } 
            // mark state as changed
            SET_FLAG_VALUE(BUTTONSTATE_ISPRESSED, isPressed, _stateFlags);
            SET_FLAG_VALUE(BUTTONSTATE_HASCHANGED, isChanged, _stateFlags);
            if (isChanged) {
                _lastChangedMillis = millis();
            }
        } else {
            // TODO: add handle release correctly after Long Press
            if (isPressed && (millis() - _lastChangedMillis) > _longPressInterval && !IS_FLAG_SET(BUTTONSTATE_ISLONGPRESSED, _stateFlags)){
                SET_FLAG_VALUE(BUTTONSTATE_ISLONGPRESSED, isPressed, _stateFlags);
                SET_FLAG(BUTTONSTATE_HASCHANGED, _stateFlags);
            }
        }
    }
}


bool Button::begin(){
    bool result = true;
    if (!IS_FLAG_SET(BUTTONSTATE_ACTIVE, _stateFlags)){
        RESET_FLAG(BUTTONSTATE_ISPRESSED, _stateFlags);
        RESET_FLAG(BUTTONSTATE_HASCHANGED, _stateFlags);
        _lastChangedMillis = 0;
        _lastClickMillis = 0;

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

void Button::onInterrupt(DigitalPin * pin){
    refreshState(true);
    //LOG_INFOF("Button State: %i\n", (int)pin->digitalRead());
}