#include "ota-controller.h"

OtaController::OtaController(OtaUpdater * otaUpdater, WiFiAP * wifiAp, OtaView * view) : 
    _otaUpdater(otaUpdater),
    _wifiAp(wifiAp),
    _view(view)
{

}

bool OtaController::activate() {
    _model.clear();
    bool success = _wifiAp->turnOn()
        && _otaUpdater->enableOta(true);
    if (success) {
        _model.apName = _wifiAp->getApName();
        _model.ownIP = _wifiAp->getOwnIP();
        _model.setState(OTAMODELSTATE_ENABLED, "Waiting connection...");
    } else {
        _model.setState(OTAMODELSTATE_ERROR, "Can't enable OTA");
    }
    _view->drawModel(&_model);
    return true;
}

bool OtaController::deactivate() {
    bool success = _wifiAp->turnOff()
        && _otaUpdater->enableOta(false);

    //  TODO: log if deactivation was unsuccessful

    _model.clear();

    return true;
}

bool OtaController::handleTick() {

    // TODO: read OTA progress, display progress

}

ModeControllerHandleUserInputResult OtaController::handleUserInput(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state) {

}
