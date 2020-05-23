#include "pump_controller.h"

#define PUMPSIG_ENABLE      ((_controlFlags & PUMPCTL_ENABLE_HIGH) == PUMPCTL_ENABLE_HIGH ? HIGH : LOW)
#define PUMPSIG_DISABLE     ((_controlFlags & PUMPCTL_ENABLE_HIGH) == PUMPCTL_ENABLE_HIGH ? LOW : HIGH)
#define PUMPSIG_INIT        ((_controlFlags & PUMPCTL_START_ON) == PUMPCTL_START_ON ? PUMPSIG_ENABLE : PUMPSIG_DISABLE)
#define PUMPSATE_ACTIVE     ((_controlFlags & PUMPCTL_START_ON) == PUMPCTL_START_ON ? PUMPSIG_ENABLE : PUMPSIG_DISABLE)

#define IS_FLAG_SET(flag, x) ((x &flag) == flag)
#define SET_FLAG(flag, x) (x|=flag)
#define RESET_FLAG(flag, x) (x=(x & ~flag))

PumpController::PumpController(uint8_t controlPin, uint8_t controlFlags, uint16_t maxWorkDurationSec):
    _controlPin(controlPin),
    _controlFlags(controlFlags),
    _stateFlags(PUMPSTATE_INACTIVE),
    _maxWorkDurationSec(maxWorkDurationSec),
    _totalStartCount(0),
    _totalWorkedSec(0),
    _lastStarted(0),
    _lastStopped(0)
{
}

bool PumpController::begin(){ 
    bool result = true;
    if (!IS_FLAG_SET(PUMPSTATE_ACTIVE, _stateFlags)) {
        result = changePumpState(PUMPSIG_INIT == PUMPSIG_ENABLE, _maxWorkDurationSec);
        if (result) {
            SET_FLAG(PUMPSTATE_ACTIVE, _stateFlags);
        }
    }
    return result;
}

bool PumpController::end(){
    bool result = true;
    if (IS_FLAG_SET(PUMPSTATE_ACTIVE, _stateFlags)) {
        result = changePumpState(false, 0);;
        if (result) {
            RESET_FLAG(PUMPSTATE_ACTIVE, _stateFlags);
        }
    }
    return result;
}

bool PumpController::isWorking(){
    return readPumpState();
}

bool PumpController::startPump(uint16_t duration){
    // if controller instance inactive, then pump can't be stoped
    if (!IS_FLAG_SET(PUMPSTATE_ACTIVE, _stateFlags))
        return false;

    uint16_t actualDuration = 
        (duration == 0 || duration > _maxWorkDurationSec) 
        ? _maxWorkDurationSec 
        : duration;
    return changePumpState(true, actualDuration);
}

bool PumpController::stopPump(){
    // if controller instance inactive, then pump can't be stoped
    if (!IS_FLAG_SET(PUMPSTATE_ACTIVE, _stateFlags))
        return false;

    return changePumpState(false, 0);
}


bool PumpController::changePumpState(bool isWorking, uint16_t duration) {
    digitalWrite(_controlPin, isWorking ? PUMPSIG_ENABLE : PUMPSIG_DISABLE);
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

bool PumpController::readPumpState(){
    return digitalRead(_controlPin) == PUMPSIG_ENABLE;
}