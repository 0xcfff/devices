#include "navigation-controller.h"

MainController::MainController(uint8_t modePin, uint8_t confirmPin, uint8_t cancelPin, NavigationView * modeSelectionPresenter, std::vector<std::pair<ModeDescription *, ModeController *>> * childControllers) :
    _modePin(modePin),
    _confirmPin(confirmPin),
    _cancelPin(cancelPin),
    _defaultControllerIndex(0),
    _currentControllerIndex(ROOTCTL_NO_MODECTL_SELECTED),
    _view(modeSelectionPresenter)
{
    if (childControllers != nullptr) {
        for(int i = 0; i < childControllers->size(); i++) {
            _modeControllers.push_back(childControllers->at(i));
        }
    }
}

MainController::~MainController()
{
    _modeControllers.clear();
    _currentControllerIndex = ROOTCTL_NO_MODECTL_SELECTED;
}


bool MainController::handle(){

    return true;
}

void MainController::addChildModeController(ModeDescription * controllerInfo, ModeController * controller){
    std::pair<ModeDescription*, ModeController*> pair = std::pair<ModeDescription*, ModeController*>(controllerInfo, controller);
    _modeControllers.push_back(pair);
    if ((controllerInfo->flags & MODEDESCR_FLAG_DEFAULTMODE) == MODEDESCR_FLAG_DEFAULTMODE) {
        _defaultControllerIndex = _modeControllers.size() - 1;
    }
}