#include <Arduino.h>
#include <FunctionalInterrupt.h>

#include "arduino-digital-pin.h"

#include "shared_macroses.h"


ArduinoDigitalPin::ArduinoDigitalPin(uint8_t pinNumber, uint8_t pinMode, uint8_t configFlags):
    _pinNumber(pinNumber),
    _pinMode(pinMode),
    _configFlags(configFlags),
    _stateFlags(ARDUINODIGPINSTATE_EMPTY)
{
    if (pinMode == OUTPUT || pinMode == OUTPUT_OPEN_DRAIN) {
        SET_FLAG(ARDUINODIGPINSTATE_SUPPORTSDIRECTWRITE, _stateFlags);
    }
    if (pinMode == INPUT || pinMode == INPUT_PULLUP || pinMode == INPUT_PULLDOWN_16) {
        SET_FLAG(ARDUINODIGPINSTATE_SUPPORTSDIRECTREAD, _stateFlags);
    }
}


std::string ArduinoDigitalPin::displayName(){
    char buff[8];
    snprintf(buff, sizeof(buff), "pin:%i", _pinNumber);
}

uint8_t ArduinoDigitalPin::supportedFeatures(){
    return PIN_FEATURE_DIGITALREAD 
        | (IS_FLAG_SET(ARDUINODIGPINSTATE_SUPPORTSDIRECTWRITE, _stateFlags) ? PIN_FEATURE_DIGITALWRITE : 0 )
        | (IS_FLAG_SET(ARDUINODIGPINCFG_HARDWAREINTERRUPTS_ENABLE, _configFlags) ? PIN_FEATURE_ATTACHINTERRUPTHANDLER : 0);
}

bool ArduinoDigitalPin::digitalRead(){
    if (IS_FLAG_SET(ARDUINODIGPINSTATE_ACTIVE, _stateFlags)) {
        if (IS_FLAG_SET(ARDUINODIGPINSTATE_SUPPORTSDIRECTREAD, _stateFlags)) {
            return ::digitalRead(_pinNumber) == HIGH;
        } 
        return IS_FLAG_SET(ARDUINODIGPINSTATE_ISSET, _stateFlags);
    }
    return false;
}

bool ArduinoDigitalPin::digitalWrite(bool isHigh){
    if (IS_FLAG_SET(ARDUINODIGPINSTATE_ACTIVE, _stateFlags)) {
        if (IS_FLAG_SET(ARDUINODIGPINSTATE_SUPPORTSDIRECTWRITE, _stateFlags)) {
            ::digitalWrite(_pinNumber, isHigh ? HIGH : LOW);
            SET_FLAG_VALUE(ARDUINODIGPINSTATE_ISSET, isHigh, _stateFlags);
            return true;
        }
    }
    return false;
}

bool ArduinoDigitalPin::attachInterruptHandler(std::function<void(DigitalPin *)> handler, uint8_t mode){
    if (IS_FLAG_SET(ARDUINODIGPINCFG_HARDWAREINTERRUPTS_ENABLE, _configFlags)) {

        std::function<void(void)> wrappedHandler = std::bind(handler, this);
        // TODO: make it compatible with raw arduino
        attachInterrupt(digitalPinToInterrupt(_pinNumber), wrappedHandler, mode);

        return true;
    }
    return false;
}
bool ArduinoDigitalPin::detachInterruptHandler(){
    if (IS_FLAG_SET(ARDUINODIGPINCFG_HARDWAREINTERRUPTS_ENABLE, _configFlags)) {
        detachInterrupt(_pinNumber);
        return true;
    }
    return false;
}

bool ArduinoDigitalPin::begin(){
    if (!IS_FLAG_SET(ARDUINODIGPINSTATE_ACTIVE, _stateFlags)){
        if (_pinMode != ARDUINODIGPINMODE_UNDEFINED) {
            pinMode(_pinNumber, _pinMode);
        }
        if (IS_FLAG_SET(ARDUINODIGPINSTATE_SUPPORTSDIRECTWRITE, _stateFlags)) {
            if (IS_FLAG_SET(ARDUINODIGPINCFG_DEFAULT_HIGH, _configFlags)){
                ::digitalWrite(_pinNumber, HIGH);
            }
            if (IS_FLAG_SET(ARDUINODIGPINCFG_DEFAULT_LOW, _configFlags)){
                ::digitalWrite(_pinNumber, LOW);
            }
        } 
        if (!IS_FLAG_SET(ARDUINODIGPINSTATE_SUPPORTSDIRECTREAD, _stateFlags)) {
            if (IS_FLAG_SET(ARDUINODIGPINCFG_DEFAULT_HIGH, _configFlags)){
                SET_FLAG_VALUE(ARDUINODIGPINSTATE_ISSET, true, _stateFlags);
            }
            if (IS_FLAG_SET(ARDUINODIGPINCFG_DEFAULT_LOW, _configFlags)){
                    SET_FLAG_VALUE(ARDUINODIGPINSTATE_ISSET, false, _stateFlags);
            }
        }

        SET_FLAG(ARDUINODIGPINSTATE_ACTIVE, _stateFlags);
    }
    return true;
}

bool ArduinoDigitalPin::end(){
    if (IS_FLAG_SET(ARDUINODIGPINSTATE_ACTIVE, _stateFlags)){
        RESET_FLAG(ARDUINODIGPINSTATE_ACTIVE, _stateFlags);
    }
    return true;
}