#include "navigation-controller.h"

MainController::MainController(uint8_t modePin, uint8_t confirmPin, uint8_t cancelPin, NavigationView * modeSelectionPresenter, std::vector<std::pair<NavigationTargetDescriptor *, ModeController *>> * childControllers) :
    _modePin(modePin),
    _confirmPin(confirmPin),
    _cancelPin(cancelPin),
    _view(modeSelectionPresenter)
{
    if (childControllers != nullptr) {
        for(int i = 0; i < (int)childControllers->size(); i++) {
            std::pair<NavigationTargetDescriptor*, ModeController*> pair = childControllers->at(i);
            addChildModeController(pair.first, pair.second);
        }
    }
}

MainController::~MainController()
{
    _modeControllers.clear();
    _model.clear();
}

bool MainController::handle(){

    // check buttons changes from input driver
    // in no button changes, check UI redraw is needed due to new ViewModel state
    
    return true;
}

void MainController::addChildModeController(NavigationTargetDescriptor * controllerInfo, ModeController * controller){
    
    uint8_t modeIndex = _model.addTargetDescriptor(controllerInfo);
    _modeControllers[modeIndex] = controller;
}