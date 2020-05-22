#include "pump_controller.h"

#define PUMPSIG_ENABLE      ((_controlFlags & PUMPCTL_ENABLE_HIGH) == PUMPCTL_ENABLE_HIGH ? HIGH : LOW)
#define PUMPSIG_DISABLE     ((_controlFlags & PUMPCTL_ENABLE_HIGH) == PUMPCTL_ENABLE_HIGH ? LOW : HIGH)
#define PUMPSIG_INIT        ((_controlFlags & PUMPCTL_START_ON) == PUMPCTL_START_ON ? PUMPSIG_ENABLE : PUMPSIG_DISABLE)

PumpController::PumpController(uint8_t controlPin, uint8_t controlFlags, uint16_t maxWorkDurationSec):
    _lastStarted(0),
    _lastStopped(0),
    _isWorking(false),
    _controlPin(controlPin),
    _controlFlags(controlFlags),
    _maxWorkDurationSec(maxWorkDurationSec),
    _totalStartCount(0),
    _totalWorkedSec(0)
{
}

bool PumpController::begin(){ 
    changePumpState(PUMPSIG_INIT == PUMPSIG_ENABLE, _maxWorkDurationSec);
}

bool PumpController::isWorking(){
    return _isWorking;
}

bool PumpController::startPump(uint16_t duration){
    uint16_t actualDuration = 
        (duration == 0 || duration > _maxWorkDurationSec) 
        ? _maxWorkDurationSec 
        : duration;
    return changePumpState(true, actualDuration);
}

bool PumpController::stopPump(){
    if (!_isWorking)
        return false;
    return changePumpState(false, 0);
}


bool PumpController::changePumpState(bool isWorking, uint16_t duration) {
    digitalWrite(_controlPin, isWorking ? PUMPSIG_ENABLE : PUMPSIG_DISABLE);
    _isWorking = isWorking;
    _workDurationSec = duration;
    if (_isWorking) {
        _lastStarted = millis();
        _totalStartCount++;
    } else {
        _lastStopped = millis();
        _totalWorkedSec += (uint32_t)((_lastStarted - _lastStopped) / 1000);
    }
    return true;
}