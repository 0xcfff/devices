#include <assert.h>

#include <macro-logs.h>

#include <rfframe.h>
#include <rfcommand.h>

#include "pump_messages.h"
#include "commands_processor.h"

CommandsProcessor::CommandsProcessor(RF24 & radio, Relay & waterPumpRelay):
    _radio(radio),
    _channel( new RFChannel(&radio, 255, 10 * 1000)),
    _waterPumpRelay(waterPumpRelay),
    _active(false)
{
}

bool CommandsProcessor::begin()
{
    _waterPumpRelay.begin();
    _channel->openRedingPipe(1, 0xCCCCCCC1C0LL);
    _channel->begin();
    _radio.startListening();
    // TODO: remove this test hardcode
    _active = true;
    return true;
}

bool CommandsProcessor::end()
{
    _waterPumpRelay.end();
    //_radio.stopListening();
    _channel->end();
    _active = false;
    return true;
}


bool CommandsProcessor::handle()
{    
    bool result = true;
    // TODO: remove this test hardcode
    _channel->handle();
    if (_channel->getContentAvailable()) {
        uint8_t pipe;
        RFChannelContent content;

        if (_channel->getContentAvailable(&pipe, &content)){
            size_t contentSize = _channel->getContentSize(pipe);
            uint8_t buff[contentSize];
            RFFrameHeader frameHeader;            
            if (!_channel->readHeader(pipe, &frameHeader) 
                || !_channel->receiveContent(pipe, buff, contentSize))
            {
                LOG_WARNLN("Can't receive content");
                result = false;
            } 
            else 
            {
                LOG_INFOF("%s message received, Flags: 0x%02x, From: 0x%08llX, To: 0x%08llX\n", 
                    content == RFCHANNEL_CONTENT_COMMAND ? "Command" : "Data", 
                    (int)frameHeader.flags, 
                    (uint64_t)frameHeader.fromAddress, 
                    (uint64_t)frameHeader.toAddress
                    );

                if (content == RFCHANNEL_CONTENT_COMMAND) {
                    result = dispatchSysCommand(&frameHeader, buff, contentSize);
                    if (!result) {
                        reportMalformedInboundMessage(buff, contentSize);
                    }

                } else {
                    if (!validateMessageSize(contentSize))
                    {
                        result = false;
                    } else {
                        result = dispatchAppCommand(&frameHeader, buff, contentSize);
                        if (!result) {
                            reportMalformedInboundMessage(buff, contentSize);
                        }
                    }
                }
            }
            
        }
    }
    return result;
}

bool CommandsProcessor::dispatchSysCommand(RFFrameHeader* frameHeader, void * commandMessage, uint16_t messageSize){
    uint8_t * messageBuffer = (uint8_t *)commandMessage;
    uint8_t command = *messageBuffer;
    messageBuffer++;
    messageSize--;

    switch (command)
    {
        case RFCOMMAND_PING:
            _channel->sendCommand(frameHeader->toAddress, frameHeader->fromAddress, frameHeader->sequenceId, RFCOMMAND_PONG, nullptr, 0, true);
            break;
        default:
            break;
    }
}

bool CommandsProcessor::dispatchAppCommand(RFFrameHeader* frameHeader, void * commandMessage, uint16_t messageSize){

    LOG_DINF_TXT(txtSuccess, "SUCCESS");
    LOG_DINF_TXT(txtFailure, "FAILURE");
    LOG_DINF_TXT(txtCommandFlip, "WATERPUMP_FLIP");
    LOG_DINF_TXT(txtCommandStart, "WATERPUMP_START");
    LOG_DINF_TXT(txtCommandStop, "WATERPUMP_STOP");
    LOG_DINF_TXT(txtCommandLogPattern, "%s command processed: %s\n");


    bool handled = true;

    bool statusResponseRequired = false;    
    RfRequest * pMessage = (RfRequest*)commandMessage;
    LOG_DEBUGF("Entered app message handler, command: %i\n", (int)pMessage->header.command);

    switch (pMessage->header.command)
    {
        case PUMP_FLIP: {
                bool flipPumpResult = _waterPumpRelay.flip(pMessage->body.pumpStartOrFlip.durationSec);
                LOG_INFOF(txtCommandLogPattern, txtCommandFlip, flipPumpResult ?  txtSuccess : txtFailure);
                statusResponseRequired = true;
                break;
            }
        case PUMP_START: {
                bool startPumpResult = _waterPumpRelay.turnOn(pMessage->body.pumpStartOrFlip.durationSec);
                LOG_INFOF(txtCommandLogPattern, txtCommandStart, startPumpResult ?  txtSuccess : txtFailure);
                statusResponseRequired = true;
                break;
            }
        case PUMP_STOP: {
                bool stopPumpResult = _waterPumpRelay.turnOff();
                LOG_INFOF(txtCommandLogPattern, txtCommandStop, stopPumpResult ?  txtSuccess : txtFailure);
                statusResponseRequired = true;
                break;
            }
        case PUMP_STATE: {
                statusResponseRequired = true;
                break;
            }
        default: {
                handled = false;
                break;
            }
    }

    if (statusResponseRequired) 
    {
        PumpControlStatusResponse response = {
            .pumpState = _waterPumpRelay.getState(),
            .timeSinceStartedSec = _waterPumpRelay.getTimeSinceStartedSec(),
            .timeTillAutostopSec = _waterPumpRelay.getTimeTillAutostopSec()
        };
        bool res = _channel->sendData(frameHeader->toAddress, frameHeader->fromAddress, frameHeader->sequenceId, &response, sizeof(PumpControlStatusResponse), true);
        LOG_DEBUGF("Response is sent, result: %s\n", res ? "Success" : "Failed");
    }

    return handled;
}


bool CommandsProcessor::validateMessageSize(uint16_t messageSize){
    if (messageSize < sizeof(RfRequestHeader)) 
        return false;
    return true;
}

void CommandsProcessor::reportMalformedInboundMessage(void * commandMessage, uint16_t messageSize){
    RfRequest * pMessage = (RfRequest*)commandMessage;
    Serial.printf("Unknown command received: %i\n", (int)pMessage->header.command);
}