#include "shared_macroses.h"

#include "pcf8574-digital-pin.h"

PCF8574DigitalPin::PCF8574DigitalPin(PCF857x * pcf8574, uint8_t pinNumber, uint8_t configFlags):
    _pcf8574(pcf8574),
    _pinNumber(pinNumber),
    _configFlags(configFlags),
    _stateFlags(PCF8574DIGPINSTATE_EMPTY)
{

}


uint8_t PCF8574DigitalPin::supportedFeatures(){
    return PIN_FEATURE_DIGITALREAD 
        | PIN_FEATURE_DIGITALWRITE 
        | (IS_FLAG_SET(PCF8574DIGPINCFG_SOFTWAREINTERRUPTS_ENABLE, _configFlags) ? PIN_FEATURE_ATTACHINTERRUPTHANDLER : 0);
}

bool PCF8574DigitalPin::digitalRead(){
    if (IS_FLAG_SET(PCF8574DIGPINSTATE_ACTIVE, _stateFlags)) {
        return _pcf8574->read(_pinNumber) == HIGH;
    }
    return false;
}

bool PCF8574DigitalPin::digitalWrite(bool isHigh){
    if (IS_FLAG_SET(PCF8574DIGPINSTATE_ACTIVE, _stateFlags)) {
        _pcf8574->write(_pinNumber, isHigh ? HIGH : LOW);
        return true;
    }
    return false;
}

bool PCF8574DigitalPin::attachInterruptHandler(std::function<void(DigitalPin *)> handler, uint8_t mode){
    if (IS_FLAG_SET(PCF8574DIGPINCFG_SOFTWAREINTERRUPTS_ENABLE, _configFlags)) {
        _interruptHandler = handler;
        return true;
    }
    return false;
}
bool PCF8574DigitalPin::detachInterruptHandler(){
    if (IS_FLAG_SET(PCF8574DIGPINCFG_SOFTWAREINTERRUPTS_ENABLE, _configFlags)) {
        _interruptHandler = nullptr;
        return true;
    }
    return false;
}

bool PCF8574DigitalPin::riseInterrupt(){
    if (IS_FLAG_SET(PCF8574DIGPINSTATE_ACTIVE, _stateFlags) 
        && IS_FLAG_SET(PCF8574DIGPINCFG_SOFTWAREINTERRUPTS_ENABLE, _configFlags) 
        && _interruptHandler != nullptr) {
        _interruptHandler(this);
        return true;
    }
    return false;
}

bool PCF8574DigitalPin::begin(){
    if (!IS_FLAG_SET(PCF8574DIGPINSTATE_ACTIVE, _stateFlags)){
        if (IS_FLAG_SET(PCF8574DIGPINCFG_DEFAULT_HIGH, _configFlags)){
            _pcf8574->write(_pinNumber, HIGH);
        }
        if (IS_FLAG_SET(PCF8574DIGPINCFG_DEFAULT_LOW, _configFlags)){
            _pcf8574->write(_pinNumber, LOW);
        }
        SET_FLAG(PCF8574DIGPINSTATE_ACTIVE, _stateFlags);
    }
    return true;
}

bool PCF8574DigitalPin::end(){
    if (IS_FLAG_SET(PCF8574DIGPINSTATE_ACTIVE, _stateFlags)){
        RESET_FLAG(PCF8574DIGPINSTATE_ACTIVE, _stateFlags);
    }
    return true;
}