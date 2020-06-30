#ifndef _OTA_UPDATER_H_
#define _OTA_UPDATER_H_

#include <ArduinoOTA.h>

#define OTASTATE_EMPTY     0x00
#define OTASTATE_ACTIVE    0x01
#define OTASTATE_ON        0x02

enum OtaUpdaterStatus : uint8_t {
    OTAUPDATERSTATUS_NONE                   = 0x00,
    OTAUPDATERSTATUS_DISABLED               = 0x01,
    OTAUPDATERSTATUS_WAITINGCONNECTION      = 0x02,
    OTAUPDATERSTATUS_INPROGRESS             = 0x03,
    OTAUPDATERSTATUS_COMPLETED              = 0x04,
    OTAUPDATERSTATUS_ERROR                  = 0xFF
};

class OtaUpdater {
    public:
        OtaUpdater();

        bool begin();
        bool end();

        bool handle();

        bool isEnabled();
        bool enableOta(bool enabled = true);

        OtaUpdaterStatus getStatus();
        uint8_t getProgress();
        unsigned long getLastActivityAt();

    private:
        bool init();
        void onStartOTA();
        void onErrorOTA(ota_error_t error);
        void onProgressOTA(unsigned int progress, unsigned int total);
        void onEndOTA();

    private:
        uint8_t _stateFlags;
        OtaUpdaterStatus _status;
        uint8_t _progress;
        unsigned long _lastActivityAt;


};

#endif