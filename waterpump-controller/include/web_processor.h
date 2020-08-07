#ifndef _WEB_PROCESSOR_H_
#define _WEB_PROCESSOR_H_

#include <ESP8266WebServer.h>
#include <RF24.h>
#include <relay.h>
#include <button_processor.h>


#define WEBPROCESSORSTATE_EMPTY     0x00
#define WEBPROCESSORSTATE_INIT      0x01
#define WEBPROCESSORSTATE_ACTIVE    0x02


class WebProcessor
{
    public:
        WebProcessor(ESP8266WebServer * webServer, RF24 * radio, Relay * relay, ControlButtonProcessor * cmdProcessor);

        bool begin();
        bool end();

        bool handle();

    private:
        bool setupWebHandlers();

    private:
        ESP8266WebServer * _webServer;
        RF24 * _radio;
        Relay * _relay;
        ControlButtonProcessor * _cmdProcessor;
        uint8_t _stateFlags;

};

#endif  // _WEB_PROCESSOR_H_