#ifndef _WATERPUMP_MODEL_H_
#define _WATERPUMP_MODEL_H_

#include <stdint.h>

enum WaterPumpConnectionState : uint8_t {
    WPCONNECTION_NONE           = 0,
    WPCONNECTION_CONNECTED      = 1,
    WPCONNECTION_CONNECTING     = 2,
    WPCONNECTION_NOSIGNAL       = 3,
    WPCONNECTION_RFERROR        = 4
};

struct WaterPumpModel {
    public:
        void clear();
    public:
        WaterPumpConnectionState connectionState;
};


#endif  // _WATERPUMP_MODEL_H_