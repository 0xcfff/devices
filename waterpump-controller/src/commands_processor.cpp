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
    _buffer = new uint8_t[DEFAULT_PROCESSOR_BUFFER_SIZE];
    _bufferSize = DEFAULT_PROCESSOR_BUFFER_SIZE;
}

CommandsProcessor::~CommandsProcessor()
{
    delete _buffer;
    _buffer = nullptr;
    _bufferSize = 0;
    LOG_DEBUGLN("Destroy commands processor");
}

bool CommandsProcessor::begin()
{
    _waterPumpRelay.begin();
    _radio.startListening();
    // TODO: remove this test hardcode
    _channel->openRedingPipe(1, 0xCCCCCCC1C0LL);
    _active = true;
    return true;
}

bool CommandsProcessor::end()
{
    _waterPumpRelay.end();
    _radio.stopListening();
    _active = false;
    return true;
}


bool CommandsProcessor::handle()
{    

    // TODO: remove this test hardcode
    _channel->handle();
    if (_channel->getContentAvailable()) {
        uint8_t pipe;
        RFChannelContent content;

        if (_channel->getContentAvailable(&pipe, &content)){
            size_t contentSize = _channel->getContentSize(pipe);
            uint8_t buff[contentSize];
            _channel->receiveContent(pipe, buff, contentSize);

            if (content == RFCHANNEL_CONTENT_COMMAND) {
                LOG_INFOF("Command received: %i", (int)buff[0]);
            } else {
                LOG_INFOF("Data received: %s", buff);
            }
        }
    }
    return true;

    if (_radio.available()){
        uint16_t messageSize = _radio.getDynamicPayloadSize();

        if (!validateMessageSize(messageSize))
        {
            return false;
        }

        _radio.read(_buffer, messageSize);
        bool dispatchResult = dispatchCommand(_buffer, messageSize);
        if (!dispatchResult) {
            reportMalformedInboundMessage(_buffer, messageSize);
            return false;
        }
    }
    return true;
}


bool CommandsProcessor::dispatchCommand(void * commandMessage, uint16_t messageSize){

    LOG_DINF_TXT(txtSuccess, "SUCCESS");
    LOG_DINF_TXT(txtFailure, "FAILURE");
    LOG_DINF_TXT(txtCommandFlip, "WATERPUMP_FLIP");
    LOG_DINF_TXT(txtCommandStart, "WATERPUMP_START");
    LOG_DINF_TXT(txtCommandStop, "WATERPUMP_STOP");
    LOG_DINF_TXT(txtCommandLogPattern, "%s command processed: %s\n");


    bool handled = true;

    RFFrameHeader frameHeader;
    decodeRFHeader(commandMessage, messageSize, &frameHeader);
    LOG_INFOF("Message Received, Flags: %i, From: 0x%08x, To: 0x%08x", (int)frameHeader.flags, (int)frameHeader.fromAddress, (int)frameHeader.toAddress);


    RfRequest * pMessage = (RfRequest*)commandMessage;
    switch (pMessage->header.command)
    {
        case PUMP_FLIP: {
                bool flipPumpResult = _waterPumpRelay.flip(pMessage->body.pumpStartOrFlip.durationSec);
                LOG_INFOF(txtCommandLogPattern, txtCommandFlip, flipPumpResult ?  txtSuccess : txtFailure);
                break;
            }
        case PUMP_START: {
                bool startPumpResult = _waterPumpRelay.turnOn(pMessage->body.pumpStartOrFlip.durationSec);
                LOG_INFOF(txtCommandLogPattern, txtCommandStart, startPumpResult ?  txtSuccess : txtFailure);
                break;
            }
        case PUMP_STOP: {
                bool stopPumpResult = _waterPumpRelay.turnOff();
                LOG_INFOF(txtCommandLogPattern, txtCommandStop, stopPumpResult ?  txtSuccess : txtFailure);
                break;
            }
        default: {
                handled = false;
                break;
            }
    }
    return handled;
}


bool CommandsProcessor::validateMessageSize(uint16_t messageSize){
    if (messageSize < sizeof(RfRequestHeader)) 
        return false;
    if (messageSize > _bufferSize) 
        return false;
    return true;
}

void CommandsProcessor::reportMalformedInboundMessage(void * commandMessage, uint16_t messageSize){
    RfRequest * pMessage = (RfRequest*)commandMessage;
    Serial.printf("Unknown command received: %i\n", (int)pMessage->header.command);
}