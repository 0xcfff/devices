#ifndef _COMMANDS_PROCESSOR_H_
#define _COMMANDS_PROCESSOR_H_

#include <RF24.h>

#include "relay.h" 

#define DEFAULT_PROCESSOR_BUFFER 32

class CommandsProcessor{
    public:
        CommandsProcessor(RF24 & radio, Relay & waterPump);

        ~CommandsProcessor();

        bool begin();
        bool end();
        bool handle();

    private:
        bool validateMessageSize(uint16_t messageSize);
        bool dispatchCommand(void * commandMessage, uint16_t messageSize);
        void reportMalformedInboundMessage(void * commandMessage, uint16_t messageSize);

    private:
        RF24 & _radio;
        Relay & _waterPump;

        uint8_t * _buffer;
        uint16_t _bufferSize;
        bool _active;
};

#endif