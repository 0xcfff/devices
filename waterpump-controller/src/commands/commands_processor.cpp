#include <assert.h>

#include "commands/commands_processor.h"
#include "pump_messages.h"

CommandsProcessor::CommandsProcessor(RF24 & radio):
    _radio(radio)
{
    _buffer = new uint8_t[DEFAULT_PROCESSOR_BUFFER];
    _bufferSize = DEFAULT_PROCESSOR_BUFFER;
}

CommandsProcessor::~CommandsProcessor()
{
    delete _buffer;
    _buffer = nullptr;
    _bufferSize = 0;
}

bool CommandsProcessor::begin()
{
    _radio.startListening();
    return true;
}

bool CommandsProcessor::end()
{
    _radio.stopListening();
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
    RfRequestHeader * pHeader = (RfRequestHeader*)commandMessage;
    if (pHeader->command == PUMP_INFO) {
        //TBD: handle
    }
    //TBD: handle
}


bool CommandsProcessor::validateMessageSize(uint16_t messageSize){
    if (messageSize < sizeof(RfRequestHeader)) 
        return false;
    if (messageSize > _bufferSize) 
        return false;
    return true;
}

void CommandsProcessor::reportMalformedInboundMessage(void * commandMessage, uint16_t messageSize){
    RfRequestHeader * pHeader = (RfRequestHeader*)commandMessage;
    Serial.printf("Unknown command received: %i", (int)pHeader->command);
}