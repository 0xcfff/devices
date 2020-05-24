#include <Arduino.h>

#include "relay.h"

#define RELAYSIG_ENABLE      ((_controlFlags & RELAYCTL_ENABLE_HIGH) == RELAYCTL_ENABLE_HIGH ? HIGH : LOW)
#define RELAYSIG_DISABLE     ((_controlFlags & RELAYCTL_ENABLE_HIGH) == RELAYCTL_ENABLE_HIGH ? LOW : HIGH)
#define RELAYSIG_INIT        ((_controlFlags & RELAYCTL_START_ON) == RELAYCTL_START_ON ? RELAYSIG_ENABLE : RELAYSIG_DISABLE)

#define IS_FLAG_SET(flag, x) ((x &flag) == flag)
#define SET_FLAG(flag, x) (x|=flag)
#define RESET_FLAG(flag, x) (x=(x & ~flag))

Relay::Relay(uint8_t controlPin, uint8_t controlFlags, uint16_t maxWorkDurationSec):
    _controlPin(controlPin),
    _controlFlags(controlFlags),
    _stateFlags(RELAYSTATE_EMPTY),
    _maxWorkDurationSec(maxWorkDurationSec),
    _totalStartCount(0),
    _totalWorkedSec(0),
    _lastStarted(0),
    _lastStopped(0)
{
}

bool Relay::begin(){ 
    bool result = true;
    if (!IS_FLAG_SET(RELAYSTATE_INIT, _stateFlags)){
        result = init();
        if (result) {
            SET_FLAG(RELAYSTATE_INIT, _stateFlags);
        }
    }
    if (result && !IS_FLAG_SET(RELAYSTATE_ACTIVE, _stateFlags)) {
        result = changeRelayState(RELAYSIG_INIT == RELAYSIG_ENABLE, _maxWorkDurationSec);
        if (result) {
            SET_FLAG(RELAYSTATE_ACTIVE, _stateFlags);
        }
    }
    return result;
}

bool Relay::end(){
    bool result = true;
    if (IS_FLAG_SET(RELAYSTATE_ACTIVE, _stateFlags)) {
        result = changeRelayState(false, 0);;
        if (result) {
            RESET_FLAG(RELAYSTATE_ACTIVE, _stateFlags);
        }
    }
    return result;
}

bool Relay::getState(){
    if (!IS_FLAG_SET(RELAYSTATE_ACTIVE, _stateFlags))
        return false;
        // TODO: read pump state from flags as the pin is set to be OUTPUT, not input
    return readRelayState();
}

bool Relay::setState(bool isTurnedOn, uint16_t duration){
    return isTurnedOn 
        ? turnOn(duration)
        : turnOff();
}


bool Relay::turnOn(uint16_t duration){
    // if controller instance inactive, then pump can't be stoped
    if (!IS_FLAG_SET(RELAYSTATE_ACTIVE, _stateFlags))
        return false;

    uint16_t actualDuration = 
        (duration == 0 || duration > _maxWorkDurationSec) 
        ? _maxWorkDurationSec 
        : duration;
    return changeRelayState(true, actualDuration);
}

bool Relay::turnOff(){
    // if controller instance inactive, then pump can't be stoped
    if (!IS_FLAG_SET(RELAYSTATE_ACTIVE, _stateFlags))
        return false;

    return changeRelayState(false, 0);
}

//TODO: cover with tests
bool Relay::flip(){
    if (!IS_FLAG_SET(RELAYSTATE_ACTIVE, _stateFlags))
        return false;
    
    bool enabled = getState();
    bool result = enabled ? turnOff() : turnOn(0);
    return result;
}


bool Relay::init(){
    pinMode(_controlPin, OUTPUT);
    return true;
}


bool Relay::changeRelayState(bool isWorking, uint16_t duration) {
    digitalWrite(_controlPin, isWorking ? RELAYSIG_ENABLE : RELAYSIG_DISABLE);
    _workDurationSec = duration;
    if (isWorking) {
        _lastStarted = millis();
        _totalStartCount++;
    } else {
        _lastStopped = millis();
        _totalWorkedSec += (uint32_t)((_lastStarted - _lastStopped) / 1000);
    }
    return true;
}

bool Relay::readRelayState(){
    return digitalRead(_controlPin) == RELAYSIG_ENABLE;
}