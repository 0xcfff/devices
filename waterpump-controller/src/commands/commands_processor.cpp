#include "commands/commands_processor.h"

CommandsProcessor::CommandsProcessor(MessageSender & sender):
    _sender(sender)
{
    if (_sender.messageAvailable()) {
        
    }
}