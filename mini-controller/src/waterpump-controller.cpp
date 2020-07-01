#include "waterpump-controller.h"

WaterPumpController::WaterPumpController(WaterPumpView * presenter, RF24 * radio, uint16_t pingIntervalMsec):
    _view(presenter),
    _radio(radio),
    _state(WATERPUMPCTLSTATE_INACTIVE),
    _pingIntervalMsec(pingIntervalMsec),
    _lastPingTimeMillis(0)
{

}

bool WaterPumpController::handleTick(){
    // WaterPumpConnectionState lastConnectionState = _model.connectionState;
    // if (lastConnectionState != WPCONNECTION_CONNECTED) {
    //     _model.connectionState = testConnection();
    //     if (_model.connectionState != lastConnectionState) {
    //         _view->drawModel(_model);
    //     }
    // }
    switch (_state)
    {
        case WATERPUMPCTLSTATE_MONITORCONNECTION: {
            // TODO: temporary implementation, replace with normal RF messages handling
            if (_model.connectionState == WPCONNECTION_CONNECTING) {
                onReceiveRFMessage();
                redrawView();
            } else {
                unsigned long now = millis();
                bool shouldPing = ((now - _lastPingTimeMillis) > _pingIntervalMsec);
                refreshConnectionState(shouldPing);
                if (shouldPing) {
                    _lastPingTimeMillis = now;
                }
            }
            
            break;
        }
        
        default:
            break;
    }
    return true;
}

ModeControllerHandleUserInputResultData WaterPumpController::handleUserInput(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state)
{

}

bool WaterPumpController::activate(){
    _radio->powerUp();
    _model.clear();

    _state = WATERPUMPCTLSTATE_MONITORCONNECTION;
    refreshConnectionState(true);
    _lastPingTimeMillis = millis();
    redrawView();

    return true;
}

bool WaterPumpController::deactivate(){
    return true;
}

bool WaterPumpController::redrawView(){
    _view->drawModel(&_model);
    return true;
}

bool WaterPumpController::refreshConnectionState(bool withPing){
    auto oldCnnState = _model.connectionState;
    auto cnnState = WPCONNECTION_NONE;
    
    // first verify that RF chip is functioning properly
    if (cnnState == WPCONNECTION_NONE) {
        if (!_radio->isChipConnected()) {
            cnnState = WPCONNECTION_RFERROR;
        }
    }

    // check connectivity on the channel level
    if (cnnState == WPCONNECTION_NONE) {
        _radio->startListening();
        delayMicroseconds(128);
        _radio->stopListening();
        if (!_radio->testCarrier()) {
            if (!_radio->testRPD()) {
                cnnState = WPCONNECTION_NOSIGNAL;
            } else {
                cnnState = WPCONNECTION_NOISESIGNALONLY;
            }
        }
    }
    
    // check connectivity on end device level
    if (cnnState == WPCONNECTION_NONE) {
        if (withPing) {
            // TODO: implement ping functionality here
            cnnState = WPCONNECTION_CONNECTED;
        } else {
            cnnState = (oldCnnState == WPCONNECTION_CONNECTED) 
                ? WPCONNECTION_CONNECTED 
                : WPCONNECTION_CONNECTING;
        }
    }

    _model.connectionState = cnnState;
    
    return oldCnnState != cnnState;
}

void WaterPumpController::onReceiveRFMessage() {
    // TODO: implement properly
    _model.connectionState = WPCONNECTION_CONNECTED;
}