#ifndef _WIFI_AP_H_
#define _WIFI_AP_H_

#include <stdint.h>

#define WIFIAP_STATE_EMPTY      0
#define WIFIAP_STATE_ENABLED    1


class WiFiAP {
    public:
        WiFiAP(const char * apName = nullptr);

        const char * getApName();
        bool setApName(const char * apName = nullptr);

        uint32_t getOwnIP();

        bool begin();
        bool end();

        bool getState();
        bool setState(bool enabled);
        bool turnOn();
        bool turnOff();

    private:
        bool changeWiFiAPState(bool enabled);

    private:
        const char * _apName;
        uint8_t _stateFlags;

};


#endif