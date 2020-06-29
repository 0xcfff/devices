#ifndef _OTA_MODEL_H_
#define _OTA_MODEL_H_

#include <stdint.h>

#define OTAMODELSTATE_NONE                  0x00
#define OTAMODELSTATE_ENABLED               0x01
#define OTAMODELSTATE_INPROGRESS            0x02
#define OTAMODELSTATE_UPDATECOMPLETED       0x04
#define OTAMODELSTATE_ERROR                 0x08
#define OTAMODELSTATE_DISABLED              0x00



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