#include <assert.h>

#include "pump_messages.h"
#include "commands_processor.h"

CommandsProcessor::CommandsProcessor(RF24 & radio, Relay & waterPumpRelay):
    _radio(radio),
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
}

bool CommandsProcessor::begin()
{
    _waterPumpRelay.begin();
    _radio.startListening();
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
    bool handled = true;
    RfRequest * pMessage = (RfRequest*)commandMessage;
    switch (pMessage->header.command)
    {
        case PUMP_START: {
                bool startPumpResult = _waterPumpRelay.turnOn(pMessage->body.pumpStartOrFlip.durationSec);
                // TODO: Log result
                break;
            }
        case PUMP_STOP: {
                bool stopPumpResult = _waterPumpRelay.turnOff();
                // TODO: Log result
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
    Serial.printf("Unknown command received: %i", (int)pMessage->header.command);
}