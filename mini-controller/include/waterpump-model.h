#ifndef _WATERPUMP_MODEL_H_
#define _WATERPUMP_MODEL_H_

#include <stdint.h>

enum WaterPumpConnectionState : uint8_t {
    WPCONNECTION_NONE                   = 0,
    WPCONNECTION_CONNECTING             = 1,
    WPCONNECTION_CONNECTED              = 2,
    WPCONNECTION_CONNECTEDBADSYSIGNAL   = 3,
    WPCONNECTION_NOSIGNAL               = 4,
    WPCONNECTION_NORESPONSE             = 5,
    WPCONNECTION_RFERROR                = 6
};

struct WaterPumpModel {
    public:
        void clear();
    public:
        WaterPumpConnectionState connectionState;
        bool pumpIsWorking;
        uint16_t timeSincePumpStartedSec;
        uint16_t timeTillPumpAutostopSec;        
};


#endif  // _WATERPUMP_MODEL_H_