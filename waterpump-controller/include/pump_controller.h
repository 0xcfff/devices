#ifndef _PUMP_CONTROLLER_H_
#define _PUMP_CONTROLLER_H_

#include <stdint.h>

#include <Arduino.h>

#define PUMPCTL_ENABLE_HIGH     0x0001
#define PUMPCTL_ENABLE_LOW      0x0000
#define PUMPCTL_START_ON        0x0002
#define PUMPCTL_START_OFF       0x0000

class PumpController{
    public:
        PumpController(uint8_t controlPin, uint8_t controlFlags, uint16_t maxWorkDurationSec);

        bool begin();
        bool end();

        bool isWorking();
        bool startPump(uint16_t duration);
        bool stopPump();

    private:
        bool changePumpState(bool isWorking, uint16_t duration);

    private:
        uint8_t _controlPin;
        uint8_t _controlFlags;
        uint16_t _maxWorkDurationSec;
        
        uint16_t _workDurationSec;
        bool _isWorking;

        uint16_t _totalStartCount;
        uint32_t _totalWorkedSec;
        unsigned long _lastStarted;
        unsigned long _lastStopped;
};

#endif