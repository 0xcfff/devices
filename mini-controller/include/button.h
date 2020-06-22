#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <stdint.h>

#include "pins.h"

#define BUTTONCFG_ENABLE_HIGH               0x01
#define BUTTONCFG_ENABLE_LOW                0x00
#define BUTTONCFG_ATTACHPININTERRUPT        0x02

#define BUTTONSTATE_EMPTY                   0x00
#define BUTTONSTATE_ACTIVE                  0x01

// Bounce interval msec
#define BUTTONDEFAULT_BOUNCE_INTERVAL         50
#define BUTTONDEFAULT_DBLCLICK_INTERVAL      200
#define BUTTONDEFAULT_LONGPRESS_INTERVAL    2000


class Button{
    public:
        Button(DigitalPin * buttonPin, uint8_t configFlags, uint16_t bounceInterval = BUTTONDEFAULT_BOUNCE_INTERVAL, uint16_t doubleClickInterval = BUTTONDEFAULT_DBLCLICK_INTERVAL, uint16_t longPressInterval = BUTTONDEFAULT_LONGPRESS_INTERVAL);

        bool begin();
        bool end();

        bool isPressed();

    private:
        ICACHE_RAM_ATTR void onInterrupt(DigitalPin * pin);

    private:
        DigitalPin * _buttonPin;
        uint8_t _configFlags;
        uint8_t _stateFlags;
        long    _lastChanged;
        uint16_t _bounceInterval;
        uint16_t _doubleClickInterval;
        uint16_t _longPressInterval;

};


#endif