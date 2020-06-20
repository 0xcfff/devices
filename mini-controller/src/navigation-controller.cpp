#include "navigation-controller.h"

MainController::MainController(uint8_t modePin, uint8_t confirmPin, uint8_t cancelPin, ModeSelectorPresenter * modeSelectionPresenter) :
    _modePin(modePin),
    _confirmPin(confirmPin),
    _cancelPin(cancelPin),
    _defaultControllerIndex(0),
    _currentControllerIndex(ROOTCTL_NO_MODECTL_SELECTED),
    _modeSelectionPresenter(modeSelectionPresenter)
{

}

MainController::~MainController()
{
    for (int i = 0; i < (int)_modeControllers.size(); i++)
    {
        ModeControllerNode * node = _modeControllers[i];
        delete node;
    }
    _modeControllers.clear();
    _currentControllerIndex = ROOTCTL_NO_MODECTL_SELECTED;
}


ButttonsProcessingResult MainController::handle(){

    

    return PROCESSOR_HANDLE_RESULT_SUCCESS;
}

void MainController::addChildModeController(ModeDescription * controllerInfo, ModeController * controller){
    ModeControllerNode * node = new ModeControllerNode();
    node->controllerInfo = controllerInfo;
    node->controllerInstance = controller;    
    _modeControllers.push_back(node);
    if ((controllerInfo->flags & MODEDESCR_FLAG_DEFAULTMODE) == MODEDESCR_FLAG_DEFAULTMODE) {
        _defaultControllerIndex = _modeControllers.size() - 1;
    }
}