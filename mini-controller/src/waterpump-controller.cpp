#include <macro-logs.h>
#include <pump_messages.h>
#include <rfcommand.h>

#include "waterpump-controller.h"

// TODO: Remove below consts when implemented in a normal way
// broadcast address - see rfaddres, the name can be missleading in this context
const uint64_t TEMP_MY_NETWORK_ADDR         = 0x10C8FFFFFFLL;
const uint64_t TEMP_MY_BROADCAST_ADDR       = 0x10C8551900LL;
const uint64_t TEMP_TARGET_ADDR             = 0xCCCCCCC1C0LL;

WaterPumpController::WaterPumpController(WaterPumpView * presenter, RF24 * radio, uint16_t pingIntervalMsec):
    _view(presenter),
    _radio(radio),
    _channel(new RFChannel(radio, 200, 10 * 1000)),
    _state(WATERPUMPCTLSTATE_INACTIVE),
    _pingIntervalMsec(pingIntervalMsec),
    _lastPingTimeMillis(0),
    _lastReceivedTimeMillis(0)
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
            
            // process RF messages if any
            _channel->handle();
            bool messageReceived = onReceiveRFMessage();
            if (messageReceived) {
                redrawView();
                _lastReceivedTimeMillis = millis();
            } 

            // request status update from Pump controller if needed
            unsigned long now = millis();
            bool shouldPing = ((now - _lastPingTimeMillis) > _pingIntervalMsec);
            if (shouldPing) {
                requestPumpStatus();
                _lastPingTimeMillis = now;
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

    _radio->printDetails();
    _radio->powerUp();
    _model.clear();
    _channel->begin();
    _channel->openRedingPipe(1, TEMP_MY_BROADCAST_ADDR);
    //_radio->startListening();
    _radio->printDetails();

    _state = WATERPUMPCTLSTATE_MONITORCONNECTION;
    requestPumpStatus();
    _lastPingTimeMillis = millis();
    _lastReceivedTimeMillis = millis() - 1;
    redrawView();

    return true;
}

bool WaterPumpController::deactivate(){
    _radio->stopListening();
    _radio->powerDown();
    // TODO: get rid of the mixed channel and radio parts
    _channel->end();
    return true;
}

bool WaterPumpController::redrawView(){
    _view->drawModel(&_model);
    return true;
}

bool WaterPumpController::requestPumpStatus(){
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
        RfRequestHeader pumpMessage = {
            .command = PUMP_STATE
        };
        
        if (!_channel->sendData(TEMP_MY_BROADCAST_ADDR, TEMP_TARGET_ADDR, 0, &pumpMessage, sizeof(RfRequestHeader), true)){
            LOG_INFOLN("Can't sent message to Waterpump Controller");
            cnnState = WPCONNECTION_NOSIGNAL;
        }
        cnnState = WPCONNECTION_CONNECTING;
    }
    
    // check connectivity on end device level
    if (cnnState != WPCONNECTION_CONNECTING || oldCnnState == WPCONNECTION_NONE) {
        _model.connectionState = cnnState;
    }
    
    return oldCnnState != cnnState;
}

bool WaterPumpController::onReceiveRFMessage() {
    bool somethingHasChanged = false;
    uint8_t pipe;
    RFChannelContent content;
    // TODO: we should either know pipe we are working with or should rely on IPs
    if (_channel->getContentAvailable(&pipe, &content)) {
        somethingHasChanged = true;

        RFFrameHeader frameHeader;
        //_channel->readHeader(pipe, &frameHeader);

        //LOG_DEBUGF("Message Received, Flags: 0x%02x, From: 0x%08llX, To: 0x%08llX\n", (int)frameHeader.flags, (uint64_t)frameHeader.fromAddress, (uint64_t)frameHeader.toAddress);

        if (content != RFCHANNEL_CONTENT_DATA) {
            LOG_DEBUGF("RF Command received, expected Data message, pipe %i\n", (int)pipe);
        } else 
        {
            size_t contentSize = _channel->getContentSize(pipe);
            // TODO: Temporary hack. Fix this, components should rely on knowing each other and knowing message formats
            if (contentSize == sizeof(PumpControlStatusResponse)) {
                PumpControlStatusResponse response;
                _channel->receiveContent(pipe, &response, sizeof(PumpControlStatusResponse));
                _model.pumpIsWorking = response.pumpState > 0;
                _model.timeSincePumpStartedSec = response.timeSinceStartedSec;
                _model.timeTillPumpAutostopSec = response.timeTillAutostopSec;

                // update signal state
                RFChannelPipeSignalLevel signal = _channel->getSignalLevel(pipe);
                if (signal == RFCHANNELSIGNALLEVEL_GOOD) {
                    _model.connectionState = WPCONNECTION_CONNECTED;
                } else {
                    _model.connectionState = WPCONNECTION_CONNECTEDBADSYSIGNAL;
                }
            } else {
                LOG_DEBUGF("Wrong content size %i\n", (int)contentSize);
            }
        }
        
        _channel->clearContent(pipe);

    } else if (millis() - _lastReceivedTimeMillis > _pingIntervalMsec + _pingIntervalMsec / 2 ) {
        _model.connectionState = WPCONNECTION_NORESPONSE;
        somethingHasChanged = true;
    }

    return somethingHasChanged;
}