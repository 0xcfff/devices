#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdint.h>

#include "U8g2lib.h"

class Display {
    public:
        Display(U8G2 * display);

        void begin();
    
        void drawImageView(uint8_t width, uint8_t height, const uint8_t * xbm, const char * label);

        U8G2 * getRawDisplay();

        bool isDirty();
        bool flush();

        bool turnOff();
        bool turnOn();

    private:
        U8G2 * _display;
        bool _isDirty;

};

#endif  // _DISPLAY_H_