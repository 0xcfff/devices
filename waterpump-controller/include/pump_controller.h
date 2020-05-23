#ifndef _PUMP_CONTROLLER_H_
#define _PUMP_CONTROLLER_H_

#include <stdint.h>

#include <Arduino.h>

#define PUMPCTL_ENABLE_HIGH     0x01
#define PUMPCTL_ENABLE_LOW      0x00
#define PUMPCTL_START_ON        0x02
#define PUMPCTL_START_OFF       0x00
// TODO: Add Autostart mode
// TODO: Add default flags set

#define PUMPSTATE_EMPTY         0x00
#define PUMPSTATE_INIT          0x01
#define PUMPSTATE_ACTIVE        0x02

#define PUMP_DEFAULT_MAX_DURATION_SEC (30 * 60)

// TODO: Rename to RelayController

class PumpController{
    public:
        PumpController(uint8_t controlPin, uint8_t controlFlags, uint16_t maxWorkDurationSec = PUMP_DEFAULT_MAX_DURATION_SEC);

        bool begin();
        bool end();
        // TODO: Add "handle"
        // TODO: Add scheduled auto stop functionality
        // TODO: Add statistics collection logic
        // TODO: Add logging

        bool isWorking();
        bool startPump(uint16_t duration);
        bool stopPump();
        bool switchPump();

    private:
        bool initPump();
        bool changePumpState(bool isWorking, uint16_t duration = 0);
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