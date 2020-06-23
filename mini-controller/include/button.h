#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <stdint.h>

#include "pins.h"

enum ButtonChange{
    BUTTON_CHANGE_NONE                      = 0x00,
    BUTTON_CHANGE_PRESSED                   = 0x01,
    BUTTON_CHANGE_RELEASED                  = 0x02
};

enum ButtonAction{
    BUTTON_ACTION_NONE                      = 0x00,
    BUTTON_ACTION_CLICK                     = 0x01,
    BUTTON_ACTION_DBLCLICK                  = 0x02,
    BUTTON_ACTION_LONGPRESS                 = 0x04
};

#define BUTTONCFG_ENABLE_HIGH               0x01
#define BUTTONCFG_ENABLE_LOW                0x00
#define BUTTONCFG_ATTACHPININTERRUPT        0x02

#define BUTTONSTATE_EMPTY                   0x00
#define BUTTONSTATE_ACTIVE                  0x01
#define BUTTONSTATE_HASCHANGED              0x02
#define BUTTONSTATE_INTERRUPTHANDELED       0x04
#define BUTTONSTATE_ISPRESSED               0x08
#define BUTTONSTATE_ISDBLCLICK              0x10
#define BUTTONSTATE_ISCLICK                 0x20
#define BUTTONSTATE_ISLONGPRESSED           0x40

// Bounce interval msec
#define BUTTONDEFAULT_BOUNCE_INTERVAL         60
#define BUTTONDEFAULT_DBLCLICK_INTERVAL      300
#define BUTTONDEFAULT_LONGPRESS_INTERVAL    2000


class Button{
    public:
        Button(DigitalPin * buttonPin, uint8_t configFlags, uint16_t bounceInterval = BUTTONDEFAULT_BOUNCE_INTERVAL, uint16_t doubleClickInterval = BUTTONDEFAULT_DBLCLICK_INTERVAL, uint16_t longPressInterval = BUTTONDEFAULT_LONGPRESS_INTERVAL);

        bool begin();
        bool end();

        bool tick();

        bool isPressed();
        bool isReleased();

        bool isChanged();
        bool isActionPerformed();

        ButtonChange getButtonChange();
        ButtonAction getButtonAction();

    private:
        void refreshState(bool isInterrupt);

        ICACHE_RAM_ATTR void onInterrupt(DigitalPin * pin);


    private:
        DigitalPin * _buttonPin;
        uint8_t _configFlags;
        uint8_t _stateFlags;
        unsigned long _lastChangedMillis;
        unsigned long _lastClickMillis;
        uint16_t _bounceInterval;
        uint16_t _doubleClickInterval;
        uint16_t _longPressInterval;

};


#endif