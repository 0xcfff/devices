#include <macro-flags.h>
#include <macro-logs.h>

#include <LittleFS.h>

#include "web_processor.h"

// TODO: Rewrite to AsyncWebServer

WebProcessor::WebProcessor(ESP8266WebServer * webServer, RF24 * radio, Relay * relay, ControlButtonProcessor * cmdProcessor) :
    _webServer(webServer),
    _radio(radio),
    _relay(relay),
    _cmdProcessor(cmdProcessor),
    _stateFlags(WEBPROCESSORSTATE_EMPTY)
{

}



bool WebProcessor::begin()
{
    bool result = true;
    if (!IS_FLAG_SET(WEBPROCESSORSTATE_INIT, _stateFlags)) 
    {
        result = setupWebHandlers();
        SET_FLAG_VALUE(WEBPROCESSORSTATE_INIT, result, _stateFlags);
    }

    if (result && !IS_FLAG_SET(WEBPROCESSORSTATE_ACTIVE, _stateFlags)) {
        _webServer->begin();
        SET_FLAG(WEBPROCESSORSTATE_ACTIVE, _stateFlags);
    }
    return result;
}

bool WebProcessor::end()
{
    bool result = true;
    if (IS_FLAG_SET(WEBPROCESSORSTATE_ACTIVE, _stateFlags)) {
        _webServer->stop();
        RESET_FLAG(WEBPROCESSORSTATE_ACTIVE, _stateFlags);
    }
    return result;
}

bool WebProcessor::handle()
{
    bool result = false;
    if (IS_FLAG_SET(WEBPROCESSORSTATE_ACTIVE, _stateFlags)) {

        _webServer->handleClient();
        result = true;
    }
    return result;
}

bool WebProcessor::setupWebHandlers()
{
    auto server = _webServer;
    auto radio = _radio;
    auto relay = _relay;
    auto cmdProcessor = _cmdProcessor;

    server->on("/", [server](){
        String webPage;
        webPage += "<h1>ESP8266 Web Server</h1>";
        webPage += "<p><a href=\"/info\">Info</a></p>";
        webPage += "<p><a href=\"/pump\">Pump</a></p>";
        webPage += "<p><a href=\"/rf24\">RF24</a></p>";
        webPage += "<p><a href=\"/test\">Test Page</a></p>";
        server->send(200, "text/html", webPage);
    });
    server->on("/rf24", [server, radio](){
        char buff[40];
        String webPage;
        webPage += "<h1>ESP8266 Web Server</h1>";
        webPage += "<p><a href=\"/\">Home</a></p>";
        webPage += "<p>";
        // RF channel
        webPage += "Channel: ";
        sprintf(buff, "%i", (int)radio->getChannel());
        webPage += buff;
        webPage += "<br/>";
        // RF Data Rate
        webPage += "Data Rate: ";
        auto dataRate = radio->getDataRate();
        auto dataRateStr = dataRate == RF24_250KBPS ? "250KBPS" : dataRate == RF24_1MBPS ? "1MBPS" : dataRate == RF24_2MBPS ? "2MBPS" : "Unknown";
        webPage += dataRateStr;
        webPage += "<br/>";
        // RF isChipConnected
        webPage += "Chip Connected: ";
        webPage += (radio->isChipConnected() ? "YES" : "NO");
        webPage += "<br/>";
        // RF isChipConnected
        webPage += "IsFailure: ";
        webPage += (radio->failureDetected ? "YES" : "NO");
        webPage += "<br/>";

        webPage += "</p>";
        server->send(200, "text/html", webPage);
    });
    server->on("/api/ota", HTTP_POST, [cmdProcessor](){
        cmdProcessor->enableOtaMode(true);
    });
    server->on("/api/restart", HTTP_POST, [](){
        LOG_INFOLN("ESP Restart requested from Web API");        
        ESP.restart();
    });
    server->on("/info", [server](){
        char buff[40];
        String webPage;
        webPage += "<h1>ESP8266 Web Server</h1>";
        webPage += "<p><a href=\"/\">Home</a></p>";
        webPage += "<p>";
        // RF isChipConnected
        webPage += "Uptime: ";
        auto uptime = millis();
        auto hours = uptime / 1000 / 60 / 60;
        auto minutes = (uptime / 1000 / 60) % 60;
        auto seconds = (uptime / 1000) % 60;
        if (hours > 0) {
            sprintf(buff, "%i hours", (int)hours);
            webPage += buff;
        }
        if (hours > 0 || minutes > 0) {
            sprintf(buff, "%i minutes", (int)minutes);
            webPage += buff;
        }
        sprintf(buff, "%i seconds", (int)seconds);
        webPage += buff;
        webPage += "<br/>";

        webPage += "</p>";
        server->send(200, "text/html", webPage);
    });
    server->on("/pump", [server, relay](){
        char buff[40];
        String webPage;
        webPage += "<h1>ESP8266 Web Server</h1>";
        webPage += "<p><a href=\"/\">Home</a></p>";
        webPage += "<p>";
        // Relay State
        bool isRelayWorking = relay->getState();
        webPage += "Relay State: ";
        webPage += (isRelayWorking ? "Working" : "Stopped");
        webPage += "<br/>";
        if (isRelayWorking) {
            // Work Left
            webPage += "Work Left: ";
            auto workLeftTotalSec = relay->getTimeTillAutostopSec();
            auto workLeftMin = workLeftTotalSec / 60;
            auto workLeftSec = workLeftTotalSec % 60;
            if (workLeftMin > 0) {
                sprintf(buff, "%i min", (int)workLeftMin);
                webPage += buff;
            }
            sprintf(buff, "%i sec", (int)workLeftSec);
            webPage += buff;

            webPage += "<br/>";
        }

        webPage += "</p>";
        server->send(200, "text/html", webPage);
    });
    server->on("/test", [server](){
        auto file = LittleFS.open("/test.html", "r");
        file.seek(0);
        auto page = file.readString();
        file.close();
        server->send(200, "text/html", page);
    });
    server->on("/list", [server](){
        String result;

        result += "LittleFS Check:";
        result += (LittleFS.check() ? "YES" : "NO");
        result += "\n";

        auto dir = LittleFS.openDir("/");
        int cnt = 0;
        while (true)
        {
            result += "Dir: ";
            result += dir.fileName();
            result += "(";
            result += dir.fileCreationTime();
            result += ")";
            result += "\n";
            if (!dir.next()|| cnt++ > 10){
                break;
            }
        }
        server->send(200, "text/html", result);
    });
    return true;
}
