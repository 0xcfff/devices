#include <Arduino.h>

class RF24;

class MessageSender{
    public:
        MessageSender(RF24 &channel, uint8_t *pipe);
        
        bool messageAvailable();
        bool handle();
        int getMessageSize();
        void * lockMessage();
        void readMessage(void * buff, int size);

    private:
        RF24 & _channel;
        uint8_t *_pipe;

};