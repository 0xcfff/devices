#include "navigation-view.h"

NavigationView::NavigationView(Display * view):
    _display(view)
{

}

bool NavigationView::drawModeSplash(NavigationTargetDescriptor * modeDescriptor){
    _display->drawImageView(
        modeDescriptor->splashWidth,
        modeDescriptor->splashHeight,
        modeDescriptor->splashScreenXBM,
        modeDescriptor->modeName
    );
    return true;
}