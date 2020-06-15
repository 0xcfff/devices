#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <stdint.h>

#define BUTTONCFG_ENABLE_HIGH      0x01
#define BUTTONCFG_ENABLE_LOW       0x00

#define BUTTONSTATE_EMPTY          0x00
#define BUTTONSTATE_ACTIVE         0x01


class Button{
    public:
        Button(uint8_t buttonPin, uint8_t configFlags);

        bool begin();
        bool end();

        bool getState();

    private:
        bool init();

    private:
        uint8_t _buttonPin;
        uint8_t _configFlags;
        uint8_t _stateFlags;

};


#endif