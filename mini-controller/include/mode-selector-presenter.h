#ifndef _MODE_SELECTOR_PRESENTER_H_
#define _MODE_SELECTOR_PRESENTER_H_

#include <stdint.h>

#include "display.h"

#define MODEDESCR_FLAG_DEFAULTMODE 1
#define MODEDESCR_FLAG_USEXBMP     2
#define MODEDESCR_FLAG_USEPSTR     4

struct ModeDescription {
    const char * modeName;
    uint8_t flags;
    const uint8_t * splashScreenXBM;
    uint8_t splashWidth;
    uint8_t splashHeight;
};

class ModeSelectorPresenter {
    public:
        ModeSelectorPresenter(Display * display);

        bool drawModeSplash(ModeDescription * modeDescriptor);

    private:
        Display * _display;
};

#endif  // _MODE_SELECTOR_PRESENTER_H_