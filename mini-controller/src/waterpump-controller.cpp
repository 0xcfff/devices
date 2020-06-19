#include "waterpump-controller.h"

WaterPumpController::WaterPumpController(WaterPumpView * presenter, RF24 * radio):
    _view(presenter),
    _radio(radio)
{

}

bool WaterPumpController::handleTick(){
    WaterPumpConnectionState lastConnectionState = _model.connectionState;
    if (lastConnectionState != WPCONNECTION_CONNECTED) {
        _model.connectionState = testConnection();
        if (_model.connectionState != lastConnectionState) {
            _view->drawModel(_model);
        }
    }
    return true;
}

ModeControllerHandleUserInputResult WaterPumpController::handleUserInput(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state)
{

}

bool WaterPumpController::activate(){
    _radio->powerUp();
    _model.clear();
    return true;
}

bool WaterPumpController::deactivate(){
    return true;
}

WaterPumpConnectionState WaterPumpController::testConnection(){
    if (!_radio->testCarrier()) {
        return WPCONNECTION_NOSIGNAL;
    }
    return WPCONNECTION_CONNECTED;
}