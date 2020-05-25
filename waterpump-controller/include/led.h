#ifndef _LED_H_
#define _LED_H_

#include <stdint.h>

#define LEDCFG_ENABLE_HIGH      0x01
#define LEDCFG_ENABLE_LOW       0x00
#define LEDCFG_START_ON         0x02
#define LEDCFG_START_OFF        0x00

#define LEDSTATE_EMPTY          0x00
#define LEDSTATE_ACTIVE         0x01
#define LEDSTATE_ON             0x02


class Led{
    public:
        Led(uint8_t ledPin, uint8_t configFlags);

        bool begin();
        bool end();

        bool getState();
        bool setState(bool isTurnedOn);
        bool turnOn();
        bool turnOff();
        bool flip();

    private:
        bool init();
        bool changeLedState(bool turnedOn);

    private:
        uint8_t _ledPin;
        uint8_t _configFlags;
        uint8_t _stateFlags;

};

#endif