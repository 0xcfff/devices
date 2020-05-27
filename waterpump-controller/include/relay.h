#ifndef _RELAY_H_
#define _RELAY_H_

#include <stdint.h>

#define RELAYCFG_ENABLE_HIGH     0x01
#define RELAYCFG_ENABLE_LOW      0x00
#define RELAYCFG_START_ON        0x02
#define RELAYCFG_START_OFF       0x00
// TODO: Add Autostart mode
// TODO: Add default flags set

#define RELAYSTATE_EMPTY         0x00
#define RELAYSTATE_ACTIVE        0x01
#define RELAYSTATE_ON            0x02

#define RELAY_DURATION_INFINITE     0
#define RELAY_DURATION_MAXALLOWED   0

class Relay{
    public:
        Relay(uint8_t relayPin, uint8_t configFlags, uint16_t maxWorkDurationSec = RELAY_DURATION_INFINITE);

        bool begin();
        bool end();
        // TODO: Make independent of Arduino framework
        // TODO: Add "handle"
        // TODO: Add scheduled auto stop functionality
        // TODO: Add statistics collection logic
        // TODO: Add logging

        bool getState();
        bool setState(bool isTurnedOn, uint16_t duration = RELAY_DURATION_MAXALLOWED);
        bool turnOn(uint16_t duration = RELAY_DURATION_MAXALLOWED);
        bool turnOff();
        bool flip(uint16_t duration = RELAY_DURATION_MAXALLOWED);

        bool handle();

    private:
        bool init();
        bool changeRelayState(bool isWorking, uint16_t duration = RELAY_DURATION_MAXALLOWED);
        bool readRelayState();

    private:
        uint8_t _relayPin;
        uint8_t _configFlags;
        uint8_t _stateFlags;
        uint16_t _maxWorkDurationSec;
        
        uint16_t _workDurationSec;

        uint16_t _totalPhysicalTurnOnCount;
        uint32_t _totalTurnedOnDurationSec;
        unsigned long _lastStarted;
        unsigned long _lastStopped;
};

#endif