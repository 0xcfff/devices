#ifndef _COMMANDS_PROCESSOR_H_
#define _COMMANDS_PROCESSOR_H_

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <rfchannel.h>

#include "relay.h" 

#define DEFAULT_PROCESSOR_BUFFER_SIZE 32

class CommandsProcessor{
    public:
        CommandsProcessor(RF24 & radio, Relay & waterPumpRelay);

        bool begin();
        bool end();
        bool handle();

    private:
        bool validateMessageSize(uint16_t messageSize);
        bool dispatchSysCommand(RFFrameHeader* frameHeader, void * commandMessage, uint16_t messageSize);
        bool dispatchAppCommand(RFFrameHeader* frameHeader, void * commandMessage, uint16_t messageSize);
        void reportMalformedInboundMessage(void * commandMessage, uint16_t messageSize);

    private:
        RF24 & _radio;
        RFChannel * _channel;

        Relay & _waterPumpRelay;
        bool _active;
};

#endif