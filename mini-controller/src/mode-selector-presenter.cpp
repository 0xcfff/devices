#include "mode-selector-presenter.h"

ModeSelectorPresenter::ModeSelectorPresenter(Display * view):
    _display(view)
{

}

bool ModeSelectorPresenter::drawModeSplash(ModeDescription * modeDescriptor){
    _display->drawImageView(
        modeDescriptor->splashWidth,
        modeDescriptor->splashHeight,
        modeDescriptor->splashScreenXBM,
        modeDescriptor->modeName
    );
}