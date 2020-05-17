typedef enum PumpControlCommand : unsigned short {
    INFO        = 0x41,
    PUMP_STATE  = 0x42,
    START_PUMP  = 0x43,
    STOP_PUMP   = 0x44,
};

struct PumpControlStartBody {
    // Used to specify desired pump work duration.
    // Zero means maximum allowed. If the value specified is zero or exceeds the maximum 
    // allowed by pump control defaults, the default maximum value will be used, see defaults.h for details.
    unsigned short durationSec;
};

// TBD add proper comments according to https://google.github.io/styleguide/cppguide.html#Comments
struct RfRequest {
    unsigned short commandId;
    PumpControlCommand command;
    union {
       PumpControlStartBody startPump;
    };
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