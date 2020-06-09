#include <tnlog.h>

#include "shared_macroses.h"

#include "button_processor.h"

ControlButtonProcessor::ControlButtonProcessor(Button & controlButton, OtaUpdater & otaUpdater, WiFiAP & wifiAp, Led & stateIndicatorLed, uint16_t wifiAutoDisableSec, uint16_t buttonLongPressMsec, uint16_t ledOtaBlinkIntervalMsec) :
    _stateFlags(CTLBTNPROCSTATE_EMPTY),
    _lastLedFlipAt(0),
    _buttonLongPressMsec(buttonLongPressMsec),
    _wifiAutoDisableSec(wifiAutoDisableSec),
    _ledOtaBlinkIntervalMsec(ledOtaBlinkIntervalMsec),
    _controlButton(controlButton),
    _otaUpdater(otaUpdater),
    _wifiAp(wifiAp),
    _stateIndicatorLed(stateIndicatorLed)
{
    // Do something
}

bool ControlButtonProcessor::begin() {

    _controlButton.begin();

    bool result = 
        _otaUpdater.begin()
        && _wifiAp.begin()
        && _stateIndicatorLed.begin();

    if (result) {
        SET_FLAG(CTLBTNPROCSTATE_ACTIVE, _stateFlags);
    }
        
    return result;
}

bool ControlButtonProcessor::end() {
    bool result = 
        _otaUpdater.end()
        && _wifiAp.end()
        && _stateIndicatorLed.end();
    return result;
}

bool ControlButtonProcessor::handle() {
    bool result = false;
    if (IS_FLAG_SET(CTLBTNPROCSTATE_ACTIVE, _stateFlags)) {

        _controlButton.read();
        

        if (_controlButton.pressedFor(_buttonLongPressMsec)) {
            onControlButtonLongPress();
        }

        if (_controlButton.wasReleased()) {
            onControlButtonClick();
        }

        if (IS_FLAG_SET(CTLBTNPROCSTATE_OTAENABLED, _stateFlags)) {
            if ((millis() - _otaUpdater.getLastActivityAt()) / 1000 > _wifiAutoDisableSec){
                LOG_INFOLN(F("OTA inactivity timeout expired, disabling OTA mode..."));
                bool disabled = enableOtaMode(false)
                    && _stateIndicatorLed.turnOff();
                if (disabled) {
                    LOG_INFOLN(F("OTA mode successfully disabled."));
                } else {
                    LOG_ERRORLN(F("Error disabling OTA mode."));
                }
            }
        }

        if (IS_FLAG_SET(CTLBTNPROCSTATE_OTAENABLED, _stateFlags)) {
            if (millis() - _lastLedFlipAt > _ledOtaBlinkIntervalMsec){
                _stateIndicatorLed.flip();
                _ledOtaBlinkIntervalMsec = millis();
            }
        }

        result = true;
    }
    return result;
}

bool ControlButtonProcessor::enableOtaMode(bool enable){
    bool result = 
        _wifiAp.setState(enable)
        && _otaUpdater.enableOta(enable);
    if (result) {
        SET_FLAG_VALUE(CTLBTNPROCSTATE_OTAENABLED, enable, _stateFlags);
        LOG_DEBUGLN(enable ? F("OTA mode enabled.") : F("OTA mode disabled."));
    }
    return result;
}

void ControlButtonProcessor::onControlButtonClick(){
    if (!IS_FLAG_SET(CTLBTNPROCSTATE_OTAENABLED, _stateFlags)) {
        LOG_INFOLN(F("Single control button click detected, restart ESP..."));
        ESP.restart();
    }
}

void ControlButtonProcessor::onControlButtonLongPress(){
    if (!IS_FLAG_SET(CTLBTNPROCSTATE_OTAENABLED, _stateFlags)) {
        LOG_INFOLN(F("Long press detected. Enabled OTA mode..."));
        bool enabled = enableOtaMode(true);
        if (enabled) {
            LOG_INFOLN(F("OTA mode successfully enabled."));
        } else {
            LOG_ERRORLN(F("Error enabling OTA mode."));
        }
    }
}

