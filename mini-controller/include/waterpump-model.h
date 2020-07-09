#ifndef _WATERPUMP_MODEL_H_
#define _WATERPUMP_MODEL_H_

#include <stdint.h>

enum WaterPumpConnectionState : uint8_t {
    WPCONNECTION_NONE                   = 0,
    WPCONNECTION_CONNECTING             = 1,
    WPCONNECTION_CONNECTEDGOODSIGNAL    = 2,
    WPCONNECTION_CONNECTEDBADSYSIGNAL   = 3,
    WPCONNECTION_NOCONNECTION           = 4,
    WPCONNECTION_RFERROR                = 5
};

enum WaterPumpAction : uint8_t {
    WPACTION_STOP                       = 0,
    WPACTION_ADDMAX                     = 1,
    WPACTION_ADD10MIN                   = 2,
    WPACTION_ADD5MIN                    = 3,
    WPACTION_ADD1MIN                    = 4,
    WPACTION_DEFAULTACTION              = WPACTION_ADD10MIN,
    WPACTION_MAXACTION                  = WPACTION_ADD1MIN
};

struct WaterPumpModel {
    public:
        void clear();
        WaterPumpAction selectNextAction();
    public:
        WaterPumpConnectionState connectionState;
        WaterPumpAction selectedAction;
        bool pumpIsWorking;
        bool isConnected;
        unsigned long lastStatusUpdatedMillis;
        uint16_t timeSincePumpStartedSec;
        uint16_t timeTillPumpAutostopSec;
};


#endif  // _WATERPUMP_MODEL_H_