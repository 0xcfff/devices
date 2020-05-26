#include <Arduino.h>

#include "shared_macroses.h"
#include "log_macroses.h"

#include "relay.h"

#define RELAYSIG_ENABLE      ((_controlFlags & RELAYCFG_ENABLE_HIGH) == RELAYCFG_ENABLE_HIGH ? HIGH : LOW)
#define RELAYSIG_DISABLE     ((_controlFlags & RELAYCFG_ENABLE_HIGH) == RELAYCFG_ENABLE_HIGH ? LOW : HIGH)
#define RELAYSIG_INIT        ((_controlFlags & RELAYCFG_START_ON) == RELAYCFG_START_ON ? RELAYSIG_ENABLE : RELAYSIG_DISABLE)

Relay::Relay(uint8_t controlPin, uint8_t controlFlags, uint16_t maxWorkDurationSec):
    _controlPin(controlPin),
    _controlFlags(controlFlags),
    _stateFlags(RELAYSTATE_EMPTY),
    _maxWorkDurationSec(maxWorkDurationSec),
    _totalPhysicalTurnOnCount(0),
    _totalTurnedOnDurationSec(0),
    _lastStarted(0),
    _lastStopped(0)
{
}

bool Relay::begin(){ 
    bool result = true;
    if (!IS_FLAG_SET(RELAYSTATE_ACTIVE, _stateFlags)){
        result = init();
        if (result) {
            result = changeRelayState(RELAYSIG_INIT == RELAYSIG_ENABLE, _maxWorkDurationSec);
            if (result) {
                SET_FLAG(RELAYSTATE_ACTIVE, _stateFlags);
            }
        }
    }
    return result;
}

bool Relay::end(){
    bool result = true;
    if (IS_FLAG_SET(RELAYSTATE_ACTIVE, _stateFlags)) {
        result = changeRelayState(RELAYSIG_INIT, 0);;
        if (result) {
            RESET_FLAG(RELAYSTATE_ACTIVE, _stateFlags);
        }
    }
    return result;
}

bool Relay::getState(){
    if (!IS_FLAG_SET(RELAYSTATE_ACTIVE, _stateFlags))
        return false;
    return readRelayState();
}

bool Relay::setState(bool isTurnedOn, uint16_t duration){
    return isTurnedOn 
        ? turnOn(duration)
        : turnOff();
}


bool Relay::turnOn(uint16_t duration){
    // if relay instance is inactive, then it can't be turned on
    if (!IS_FLAG_SET(RELAYSTATE_ACTIVE, _stateFlags))
        return false;

    uint16_t actualDuration = 
        (duration == 0 || duration > _maxWorkDurationSec) 
        ? _maxWorkDurationSec 
        : duration;
    return changeRelayState(true, actualDuration);
}

bool Relay::turnOff(){
    // if relay instance is inactive, then it can't be turned off
    if (!IS_FLAG_SET(RELAYSTATE_ACTIVE, _stateFlags))
        return false;

    return changeRelayState(false, 0);
}

bool Relay::flip(uint16_t duration){
    if (!IS_FLAG_SET(RELAYSTATE_ACTIVE, _stateFlags))
        return false;
    
    bool enabled = getState();
    bool result = enabled ? turnOff() : turnOn(duration);
    return result;
}


bool Relay::handle(){
    if (!IS_FLAG_SET(RELAYSTATE_ACTIVE, _stateFlags))
        return false;

    bool result = true;
    if (_workDurationSec > 0) {
        unsigned long worked = (millis() - _lastStarted) / 1000;
        if (worked > _workDurationSec) {
            result = turnOff();
            LOG_INFOLN(F("Relay turned off automatically."));
        }
    }
    return result;
}



bool Relay::init(){
    pinMode(_controlPin, OUTPUT);
    return true;
}


bool Relay::changeRelayState(bool isWorking, uint16_t duration) {
    digitalWrite(_controlPin, isWorking ? RELAYSIG_ENABLE : RELAYSIG_DISABLE);
    if (isWorking) {
        _lastStarted = millis();
        _workDurationSec = duration;
        if (!IS_FLAG_SET(RELAYSTATE_ON, _stateFlags)) {
            _totalPhysicalTurnOnCount++;
            SET_FLAG(RELAYSTATE_ON, _stateFlags);
        }
    } else {
        _lastStopped = millis();
        _workDurationSec = 0;
        _totalTurnedOnDurationSec += (uint32_t)((_lastStarted - _lastStopped) / 1000);
        RESET_FLAG(RELAYSTATE_ON, _stateFlags);
    }
    return true;
}

bool Relay::readRelayState(){
    return IS_FLAG_SET(RELAYSTATE_ON, _stateFlags);
}