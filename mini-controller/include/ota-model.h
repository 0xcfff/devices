#ifndef _OTA_MODEL_H_
#define _OTA_MODEL_H_

#include <stdint.h>

#define OTAMODELSTATE_NONE                  0
#define OTAMODELSTATE_WAITING               1
#define OTAMODELSTATE_INPROGRESS            2
#define OTAMODELSTATE_UPDATECOMPLETED       3
#define OTAMODELSTATE_DISABLED              4
#define OTAMODELSTATE_ERROR                 0xFF



class OtaModel{
    public:
        OtaModel();

        void clear();
        void setState(uint8_t newState, char * newStatusText = nullptr);

    public:
        uint8_t otaState;
        uint8_t otaProgress;
        const char * statusText;
        const char * apName;
        uint32_t ownIP;

};

#endif  // _OTA_MODEL_H_