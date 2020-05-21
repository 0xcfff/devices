#include <RF24.h>

#include "message_sender.h"

MessageSender::MessageSender(RF24 & channel, uint8_t *pipe):
    _channel(channel),
    _pipe(pipe)
{
    
}

bool MessageSender::handle(){
    return true;
}


bool MessageSender::messageAvailable(){
    return _channel.available(_pipe);
}

int MessageSender::getMessageSize(){
    return _channel.getDynamicPayloadSize();
}

void MessageSender::readMessage(void * buff, int size){
    return _channel.read(buff, size);
}