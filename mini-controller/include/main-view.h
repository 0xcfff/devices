#ifndef _MAIN_VIEW_H_
#define _MAIN_VIEW_H_

#include <stdint.h>

#include "U8g2lib.h"

class MainView{
    public:
        MainView(U8G2 & display);
    
        void drawImageView(uint8_t width, uint8_t height, const uint8_t * xbm, const char * label);

    private:
        U8G2 & _display;

};

#endif // _MAIN_VIEW_H_