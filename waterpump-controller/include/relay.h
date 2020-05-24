#ifndef _RELAY_H_
#define _RELAY_H_

#include <stdint.h>

#define RELAYCTL_ENABLE_HIGH     0x01
#define RELAYCTL_ENABLE_LOW      0x00
#define RELAYCTL_START_ON        0x02
#define RELAYCTL_START_OFF       0x00
// TODO: Add Autostart mode
// TODO: Add default flags set

#define RELAYSTATE_EMPTY         0x00
#define RELAYSTATE_INIT          0x01
#define RELAYSTATE_ACTIVE        0x02

#define RELAY_DURATION_INFINITE     0
#define RELAY_DURATION_MAXALLOWED   0

// TODO: Rename to RelayController

class Relay{
    public:
        Relay(uint8_t controlPin, uint8_t controlFlags, uint16_t maxWorkDurationSec = RELAY_DURATION_INFINITE);

        bool begin();
        bool end();
        // TODO: Add "handle"
        // TODO: Add scheduled auto stop functionality
        // TODO: Add statistics collection logic
        // TODO: Add logging

        bool getState();
        bool setState(bool isTurnedOn, uint16_t duration = RELAY_DURATION_MAXALLOWED);
        bool turnOn(uint16_t duration = RELAY_DURATION_MAXALLOWED);
        bool turnOff();
        bool flip();

    private:
        bool initPump();
        bool changePumpState(bool isWorking, uint16_t duration = RELAY_DURATION_MAXALLOWED);
        bool readPumpState();

    private:
        uint8_t _controlPin;
        uint8_t _controlFlags;
        uint8_t _stateFlags;
        uint16_t _maxWorkDurationSec;
        
        uint16_t _workDurationSec;

        uint16_t _totalStartCount;
        uint32_t _totalWorkedSec;
        unsigned long _lastStarted;
        unsigned long _lastStopped;
};

#endif