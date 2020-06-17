#include "mode-selector-presenter.h"

ModeSelectorPresenter::ModeSelectorPresenter(MainView * view):
    _view(view)
{

}

bool ModeSelectorPresenter::drawModeSplash(ModeDescription * modeDescriptor){
    _view->drawImageView(
        modeDescriptor->splashWidth,
        modeDescriptor->splashHeight,
        modeDescriptor->splashScreenXBM,
        modeDescriptor->modeName
    );
}