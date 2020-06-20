#ifndef _NAVIGATION_VIEW_H_
#define _NAVIGATION_VIEW_H_

#include <stdint.h>

#include "display.h"
#include "navigation-model.h"

class NavigationView {
    public:
        NavigationView(Display * display);

        bool drawModeSplash(ModeDescription * modeDescriptor);

    private:
        Display * _display;
};

#endif  // _NAVIGATION_VIEW_H_