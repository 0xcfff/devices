#include <assert.h>
#include <strings.h>
#include <ESP8266WiFi.h>

#include <macro-flags.h>

#include "wifi-ap.h"

WiFiAP::WiFiAP(const char * apName) :
    _stateFlags(WIFIAP_STATE_EMPTY)
{
    setApName(apName);
}

const char * WiFiAP::getApName(){
    return _apName;
}

bool WiFiAP::setApName(const char * apName){
    bool result = true;
    char * buffer = nullptr;
    if (apName == nullptr) {
        buffer = new char[16];
        sprintf(buffer, "esp8266-%06x", ESP.getChipId());
    } else {
        buffer = new char[strlen(apName)+1];
        strcpy(buffer, apName);
    }
    _apName = buffer;

    if (getState()) {
        result = turnOff() && turnOn();
    }

    return result;
}

uint32_t WiFiAP::getOwnIP(){
    return WiFi.softAPIP().v4();
}

bool WiFiAP::begin(){
    // Nothing, WiFi AP does not require any pre-init actions
    return true;
}

bool WiFiAP::end(){
    bool result = true;
    if (getState()) {
        result = turnOff();
    }
    return result;
}

bool WiFiAP::getState(){
    return IS_FLAG_SET(WIFIAP_STATE_ENABLED, _stateFlags);
}

bool WiFiAP::setState(bool enabled){
    return enabled
        ? turnOn()
        : turnOff();
}


bool WiFiAP::turnOn(){
    return changeWiFiAPState(true);
}

bool WiFiAP::turnOff(){
    return changeWiFiAPState(false);
}


bool WiFiAP::changeWiFiAPState(bool enabled){
    bool result = true;    
    if (enabled) {
        WiFi.mode(WIFI_AP);
        result = WiFi.softAP(_apName);
        if (result) {
            SET_FLAG(WIFIAP_STATE_ENABLED, _stateFlags);
        }
    } else {
        result = WiFi.enableAP(false);
        if (result) {
            RESET_FLAG(WIFIAP_STATE_ENABLED, _stateFlags);
        }
    }
    return result;
}