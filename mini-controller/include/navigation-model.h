#ifndef _NAVIGATION_MODEL_H_
#define _NAVIGATION_MODEL_H_

#include <stdint.h>

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


#endif  // _NAVIGATION_MODEL_H_