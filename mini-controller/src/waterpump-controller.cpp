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
    _lastReceivedTimeMillis(0),
    _lastRedrawTimeMillis(0),
    _active(0)
{

}

bool WaterPumpController::handleTick()
{
    switch (_state)
    {
        case WATERPUMPCTLSTATE_MONITORCONNECTION: {
            
            bool redrawn = false;
            // process RF messages if any
            _channel->handle();
            bool messageReceived = onReceiveRFMessage();
            if (messageReceived) {
                redrawn = redrawView();
                _lastReceivedTimeMillis = millis();
            } 

            // request status update from Pump controller if needed
            unsigned long now = millis();
            bool shouldPing = ((now - _lastPingTimeMillis) > _pingIntervalMsec);
            if (shouldPing) {
                requestPumpStatus();
                _lastPingTimeMillis = now;
            }

            // update view is Pump connected and working and minimal time went sice last redraw
            if (_model.connectionState == WPCONNECTION_CONNECTEDGOODSIGNAL 
                || _model.connectionState == WPCONNECTION_CONNECTEDBADSYSIGNAL) 
            {
                if (_model.pumpIsWorking) {
                    if (!redrawn && (millis() - _lastRedrawTimeMillis) >= 500) 
                    {
                        redrawn = redrawView();
                        if (redrawn) {
                            _lastRedrawTimeMillis = millis();
                        }
                    }
                }
                else 
                {
                    if (_lastRedrawTimeMillis != 0) 
                    {
                        redrawn = redrawView();
                        _lastRedrawTimeMillis = 0;
                    }
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
    if (!_active) {
        return PROCESSOR_RESULT_NONE;
    }

    if (button == PROCESSOR_BUTTON_MENU) {
        if (action == PROCESSOR_BUTTON_ACTION_CLICK 
            && (_model.connectionState == WPCONNECTION_CONNECTEDGOODSIGNAL
            || _model.connectionState == WPCONNECTION_CONNECTEDBADSYSIGNAL)) {
            _model.selectNextAction();
            redrawView();
            return PROCESSOR_RESULT_SUCCESS;
        }
        // Do not return as handled for menu command as some controllers may want to handle long press
    }

    if (button == PROCESSOR_BUTTON_CANCEL) {
        if (action == PROCESSOR_BUTTON_ACTION_CLICK) {
            return PROCESSOR_RESULT_LEAVESTATE;
        }
        if (action == PROCESSOR_BUTTON_ACTION_LONGPRESS) {
            sendPumpCommand(WPACTION_STOP);
        }
        return PROCESSOR_RESULT_SUCCESS;
    }

    if (button == PROCESSOR_BUTTON_OK) {
        if (action == PROCESSOR_BUTTON_ACTION_CLICK 
            && (_model.connectionState == WPCONNECTION_CONNECTEDGOODSIGNAL
            || _model.connectionState == WPCONNECTION_CONNECTEDBADSYSIGNAL)) {
            sendPumpCommand(_model.selectedAction);
            redrawView();
        }
        return PROCESSOR_RESULT_SUCCESS;
    }

    return PROCESSOR_RESULT_NONE;


}

ModeControllerSystemEventResult WaterPumpController::handleSystemEvent(ModeControllerSystemEvent systemEvent)
{
    ModeControllerSystemEventResult result = PROCESSOR_EVENTRESULT_NONE;
    if (_active) {
        switch (systemEvent)
        {
            case PROCESSOR_EVENT_SLEEP:
                // TODO: make it more elegant
                deactivate();
                _active = true;
                result = PROCESSOR_EVENTRESULT_HANDLED;
                break;
            case PROCESSOR_EVENT_WAKEUP:
                // TODO: make it more elegant
                _active = false;
                activate();
                result = PROCESSOR_EVENTRESULT_HANDLED;
            default:
                break;
        }
    }
    return result;
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
    _active = true;

    return true;
}

bool WaterPumpController::deactivate(){
    _radio->stopListening();
    _radio->powerDown();
    // TODO: get rid of the mixed channel and radio parts
    _channel->end();
    _active = true;
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
            cnnState = WPCONNECTION_NOCONNECTION;
        }
        cnnState = WPCONNECTION_CONNECTING;
    }
    
    // check connectivity on end device level
    if (cnnState != WPCONNECTION_CONNECTING || oldCnnState == WPCONNECTION_NONE) {
        _model.connectionState = cnnState;
    }
    
    return oldCnnState != cnnState;
}

bool WaterPumpController::sendPumpCommand(WaterPumpAction action){
    
    bool success = true;
    RfRequest pumpMessage;
    bool shouldSend = true;

    switch (action)
    {
        case WPACTION_STOP: 
            pumpMessage.header.command = PUMP_STOP;
            break;
        case WPACTION_ADDMAX:
            pumpMessage.header.command = PUMP_START;
            pumpMessage.body.pumpStartOrFlip.durationSec = (60 * 60) + _model.timeTillPumpAutostopSec;
            break;
        case WPACTION_ADD10MIN: 
            pumpMessage.header.command = PUMP_START;
            pumpMessage.body.pumpStartOrFlip.durationSec = (10 * 60) + _model.timeTillPumpAutostopSec;
            break;
        case WPACTION_ADD5MIN:
            pumpMessage.header.command = PUMP_START;
            pumpMessage.body.pumpStartOrFlip.durationSec = (5 * 60) + _model.timeTillPumpAutostopSec;
            break;
        case WPACTION_ADD1MIN:
            pumpMessage.header.command = PUMP_START;
            pumpMessage.body.pumpStartOrFlip.durationSec = (1 * 60) + _model.timeTillPumpAutostopSec;
            break;        
        default:
            shouldSend = false;
            success = false;
            break;
    }
    if (shouldSend) {
        if (!_channel->sendData(TEMP_MY_BROADCAST_ADDR, TEMP_TARGET_ADDR, 0, &pumpMessage, sizeof(RfRequest), true)){
            LOG_INFOLN("Can't sent message to Waterpump Controller");
            _model.connectionState = WPCONNECTION_NOCONNECTION;
            success = false;
        }
    }

    return success;
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
                _model.lastStatusUpdatedMillis = millis();

                // update signal state
                RFChannelPipeSignalLevel signal = _channel->getSignalLevel(pipe);
                if (signal == RFCHANNELSIGNALLEVEL_GOOD) {
                    _model.connectionState = WPCONNECTION_CONNECTEDGOODSIGNAL;
                } else {
                    _model.connectionState = WPCONNECTION_CONNECTEDBADSYSIGNAL;
                }
            } else {
                LOG_DEBUGF("Wrong content size %i\n", (int)contentSize);
            }
        }
        
        _channel->clearContent(pipe);

    } else if (millis() - _lastReceivedTimeMillis > _pingIntervalMsec + _pingIntervalMsec / 2 ) {
        _model.connectionState = WPCONNECTION_NOCONNECTION;
        somethingHasChanged = true;
    }

    return somethingHasChanged;
}