#include <macro-logs.h>

#include "ota-controller.h"

OtaController::OtaController(OtaUpdater * otaUpdater, WiFiAP * wifiAp, uint16_t waitIntervalMsec, OtaView * view) : 
    _otaUpdater(otaUpdater),
    _wifiAp(wifiAp),
    _waitIntervalMsec(waitIntervalMsec),
    _isActive(false),
    _view(view)
{

}

bool OtaController::activate() {
    _model.clear();
    activateOta();
    _view->drawModel(&_model);
    _isActive = true;
    return true;
}

bool OtaController::deactivate() {

    deactivateOta();
    // mo matter what, cleanup state
    _model.clear();
    _isActive = false;
    return true;
}

bool OtaController::activateOta(){
    _model.clear();
    bool wifiSuccess = _wifiAp->turnOn(),
        otaSuccess = _otaUpdater->enableOta(true);
    if (wifiSuccess && otaSuccess) {
        _model.apName = _wifiAp->getApName();
        _model.ownIP = _wifiAp->getOwnIP();
        _model.otaStatus = _otaUpdater->getStatus();
    } else {
        _model.otaStatus = OTAUPDATERSTATUS_ERROR;
        _model.statusText = wifiSuccess ? "Can't enable OTA" : "Can't enable WIFI AP";
        LOG_ERRORLN(_model.statusText);
    }

    return wifiSuccess && otaSuccess;
}

bool OtaController::deactivateOta(){
    bool wifiSuccess = _wifiAp->turnOff(),
         otaSuccess = _otaUpdater->enableOta(false);
        
    if (!wifiSuccess) {
        LOG_WARNLN("Unable to turn off WIFI AP. Increased current consumption is possible.");
    }
    if (!otaSuccess) {
        LOG_WARNLN("Unable to turn off OTA.");
    }

    if (wifiSuccess && otaSuccess) {
        _model.otaStatus = _otaUpdater->getStatus();
        _model.otaProgress = _otaUpdater->getProgress();
    }

    return wifiSuccess && otaSuccess;
}

bool OtaController::handleTick() {
    // save initial state
    auto oldStatus = _model.otaStatus;
    auto oldProgress = _model.otaProgress;

    // let OTA updater handle its part
    _otaUpdater->handle();

    // verify is OTA needs to be deactivated due to user inactivity
    if (_model.otaStatus != OTAUPDATERSTATUS_DISABLED && (millis() - _otaUpdater->getLastActivityAt()) > _waitIntervalMsec) {
        deactivateOta();
    }

    // get updated state
    auto newStatus = _otaUpdater->getStatus();
    auto newProgress = _otaUpdater->getProgress();

    // redraw view in state has changed
    if (newStatus != oldStatus || newProgress != oldProgress) {
        _model.otaStatus = newStatus;
        _model.otaProgress = newProgress;
        _view->drawModel(&_model);
    }

    return true;
}

ModeControllerHandleUserInputResultData OtaController::handleUserInput(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state) {
    
    if (!_isActive) {
        if (button == PROCESSOR_BUTTON_MENU && action == PROCESSOR_BUTTON_ACTION_LONGPRESS) {
            return PROCESSOR_RESULT_ENTERSTATE;
        }
        return PROCESSOR_RESULT_NONE;
    }

    auto status = _otaUpdater->getStatus();

    if (button == PROCESSOR_BUTTON_OK) {
        if (action == PROCESSOR_BUTTON_ACTION_CLICK) {
            if (status == OTAUPDATERSTATUS_DISABLED || status == OTAUPDATERSTATUS_ERROR) {
                activateOta();
                _view->drawModel(&_model);
            }
        }
        return PROCESSOR_RESULT_SUCCESS;
    }
    if (button == PROCESSOR_BUTTON_CANCEL) {
        if (action == PROCESSOR_BUTTON_ACTION_CLICK) {
            if (status == OTAUPDATERSTATUS_WAITINGCONNECTION || status == OTAUPDATERSTATUS_ERROR || status == OTAUPDATERSTATUS_DISABLED) {
                return PROCESSOR_RESULT_LEAVESTATE;
            }
        }
        return PROCESSOR_RESULT_SUCCESS;
    }

    return PROCESSOR_RESULT_NONE;
}

