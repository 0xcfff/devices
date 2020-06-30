#ifndef _OTA_MODEL_H_
#define _OTA_MODEL_H_

#include <stdint.h>

#include <ota-updater.h>


class OtaModel{
    public:
        OtaModel();

        void clear();

    public:
        OtaUpdaterStatus otaStatus;
        uint8_t otaProgress;
        const char * statusText;
        const char * apName;
        uint32_t ownIP;

};

#endif  // _OTA_MODEL_H_