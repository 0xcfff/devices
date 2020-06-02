#ifndef _PUMP_MESSAGES_H_
#define _PUMP_MESSAGES_H_

#include <stdint.h>


enum PumpControlCommand : uint8_t {
    PUMP_INFO   = 0x41,
    PUMP_STATE  = 0x42,
    PUMP_START  = 0x43,
    PUMP_STOP   = 0x44,
    PUMP_FLIP   = 0x45,
};

struct PumpControlStartOrFlipBody {
    // Used to specify desired pump work duration.
    // Zero means maximum allowed. If the value specified is zero or exceeds the maximum 
    // allowed by pump control defaults, the default maximum value will be used, see defaults.h for details.
    unsigned short durationSec;
};

// TBD add proper comments according to https://google.github.io/styleguide/cppguide.html#Comments
struct RfRequestHeader {
    uint8_t flags;
    uint8_t command;
};

struct RfRequest {
    RfRequestHeader header;
    union {
       PumpControlStartOrFlipBody pumpStartOrFlip;
    } body;
};

struct PumpControlStartPumpBody {
    // duration to which the pump should be started
    uint16_t durationSec;
};


struct PumpControlInfoResponse {
    // UNIX format date and time when firmware was built
    unsigned long firmwareBuildTime;
    // tile since last pump control restart
    unsigned long uptimeSec;
    // number of pump turn on/off commands processed
    unsigned short pumpStateChangeCommanndsProcessed;
    // seconds ago started last time or 0
    unsigned long timeSincePumpLastStartedSec;
    // 0 - turned off, 1 - turned on
    unsigned short pumpState;
};

struct PumpControlStatusResponse {
    // 0 - turned off, 1 - turned on
    unsigned short pumpState;
    // seconds ago started or 0
    unsigned long timeSinceStartedSec;
    // seconds till automatic stop or 0
    unsigned long timeTillAutostopSec;
};


// TBD add proper comments according to https://google.github.io/styleguide/cppguide.html#Comments
struct RfResponse{
    unsigned short inRespondTo;
    union {
        PumpControlInfoResponse info;
        PumpControlStatusResponse pumpStatus;;
    };
};

#endif