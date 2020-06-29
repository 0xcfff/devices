#ifndef _OTA_UPDATER_H_
#define _OTA_UPDATER_H_

#include <ArduinoOTA.h>

#define OTASTATE_EMPTY     0x00
#define OTASTATE_ACTIVE    0x01
#define OTASTATE_ON        0x02

class OtaUpdater {
    public:
        OtaUpdater();

        bool begin();
        bool end();

        bool handle();

        bool isEnabled();
        bool enableOta(bool enabled = true);

        unsigned long getLastActivityAt();

    private:
        bool init();
        void onStartOTA();
        void onErrorOTA(ota_error_t error);
        void onProgressOTA(unsigned int progress, unsigned int total);
        void onEndOTA();

    private:
        uint8_t _stateFlags;
        unsigned long _lastActivityAt;


};

#endif