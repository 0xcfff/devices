#include "shared_macroses.h"

#include "led.h"

#define LEDSIG_ENABLE      ((_configFlags & LEDCFG_ENABLE_HIGH) == LEDCFG_ENABLE_HIGH ? HIGH : LOW)
#define LEDSIG_DISABLE     ((_configFlags & LEDCFG_ENABLE_HIGH) == LEDCFG_ENABLE_HIGH ? LOW : HIGH)
#define LEDSIG_INIT        ((_configFlags & LEDCFG_START_ON) == LEDCFG_START_ON ? LEDSIG_ENABLE : LEDSIG_DISABLE)


Led::Led(uint8_t ledPin, uint8_t configFlags) :
    _ledPin(ledPin),
    _configFlags(configFlags),
    _stateFlags(LEDSTATE_EMPTY)
{

}

bool Led::begin(){
    bool result = true;
    if (!IS_FLAG_SET(LEDSTATE_ACTIVE, _stateFlags)){
        result = init();
        if (result) {
            result = changeLedState(LEDSIG_INIT == LEDSIG_ENABLE);
        }
        if (result) {
            SET_FLAG(LEDSTATE_ACTIVE, _stateFlags);
        }
    }
    return result;
}

bool Led::end(){
    bool result = true;
    if (!IS_FLAG_SET(LEDSTATE_ACTIVE, _stateFlags)){
        result = changeLedState(LEDSIG_INIT == LEDSIG_ENABLE);
        if (result) {
            RESET_FLAG(LEDSTATE_ACTIVE, _stateFlags);
        }
    }
    return result;
}

bool Led::getState(){
    return IS_FLAG_SET(LEDSTATE_ACTIVE, _stateFlags) 
        && IS_FLAG_SET(LEDSTATE_ON, _stateFlags);
}

bool Led::setState(bool isTurnedOn){
    bool result = false;
    if (IS_FLAG_SET(LEDSTATE_ACTIVE, _stateFlags)){
        result = isTurnedOn
            ? turnOn()
            : turnOff();
    }
    return result;
}
bool Led::turnOn(){
    bool result = false;
    if (IS_FLAG_SET(LEDSTATE_ACTIVE, _stateFlags)){
        result = changeLedState(true);
    }
    return result;
}

bool Led::turnOff(){
    bool result = false;
    if (IS_FLAG_SET(LEDSTATE_ACTIVE, _stateFlags)){
        result = changeLedState(false);
    }
    return result;
}
bool Led::flip(){
    bool result = false;
    if (IS_FLAG_SET(LEDSTATE_ACTIVE, _stateFlags)){        
        result = changeLedState(!IS_FLAG_SET(LEDSTATE_ON, _stateFlags));
    }
    return result;
}

bool Led::changeLedState(bool turnedOn){
    digitalWrite(_ledPin, turnedOn ? LEDSIG_ENABLE : LEDSIG_DISABLE);
    SET_FLAG_VALUE(LEDSTATE_ON, turnedOn, _stateFlags);
    
    return true;
}

bool Led::init(){
    pinMode(_ledPin, OUTPUT);
    return true;
}