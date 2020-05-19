#ifndef _COMMANDS_PROCESSOR_H_
#define _COMMANDS_PROCESSOR_H_

#include "message_sender.h"

class CommandsProcessor{
    public:
        CommandsProcessor(MessageSender & sender);

        bool handle();

    private:
        MessageSender & _sender;
};

#endif