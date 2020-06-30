#include <macro-logs.h>
#include <macro-flags.h>

#include "navigation-controller.h"

MainController::MainController(Button * modeButton, Button * confirmButton, Button * cancelButton, NavigationView * modeSelectionPresenter, std::vector<std::pair<NavigationTargetDescriptor *, ModeController *>> * childControllers) :
    _modeButton(modeButton),
    _confirmButton(confirmButton),
    _cancelButton(cancelButton),
    _view(modeSelectionPresenter),
    _stateFlags(NAVCONTROLLERSTATE_EMPTY),
    _currentController(nullptr)
{
    _model.clear();   
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
    _modeButton->tick();
    _confirmButton->tick();
    _cancelButton->tick();

    auto modeButtonChanged = _modeButton->isChanged();
    auto confirmButtonChanged = _confirmButton->isChanged();
    auto cancelButtonChanged = _cancelButton->isChanged();

    if (modeButtonChanged || confirmButtonChanged || cancelButtonChanged) {
        ModeControllerCommandButton state = (ModeControllerCommandButton)(PROCESSOR_BUTTON_NONE 
            | (_modeButton->isPressed() ? PROCESSOR_BUTTON_MENU : 0) 
            | (_confirmButton->isPressed() ? PROCESSOR_BUTTON_OK : 0)
            | (_cancelButton->isPressed() ? PROCESSOR_BUTTON_CANCEL : 0)
            );

        LOG_INFOF("Buttons state: %i\n", (int)state);

        auto modeButtonAction = _modeButton->getButtonAction();
        auto confirmButtonAction = _confirmButton->getButtonAction();
        auto cancelButtonAction = _cancelButton->getButtonAction();

        if (!IS_FLAG_SET(NAVCONTROLLERSTATE_MODEENTERED, _stateFlags)) {
            if (modeButtonChanged && modeButtonAction == BUTTON_ACTION_CLICK) {
                LOG_INFOLN("Mode button click detected!");
                if (_model.navigateNextNavItem()) {
                    redrawView();
                }
            }
            if (confirmButtonChanged && confirmButtonAction == BUTTON_ACTION_CLICK) {
                LOG_INFOLN("Confirm button click detected!");
                auto controllerIndex = _model.getCurrentNavItemIndex();
                auto controller = _modeControllers.at(controllerIndex);
                if (controller->activate()) {
                    _currentController = controller;
                    SET_FLAG(NAVCONTROLLERSTATE_MODEENTERED, _stateFlags);
                }
            }
        } 
        else
        {
            if (modeButtonChanged && modeButtonAction == BUTTON_ACTION_CLICK) {
                LOG_INFOLN("Mode button click detected!");
                _currentController->handleUserInput(PROCESSOR_BUTTON_MENU, PROCESSOR_BUTTON_ACTION_CLICK, state);
            }
            if (confirmButtonChanged && confirmButtonAction == BUTTON_ACTION_CLICK) {
                LOG_INFOLN("Confirm button click detected!");
                _currentController->handleUserInput(PROCESSOR_BUTTON_OK, PROCESSOR_BUTTON_ACTION_CLICK, state);
            }
            if (cancelButtonChanged && cancelButtonAction == BUTTON_ACTION_CLICK) {
                LOG_INFOLN("Cancel button click detected!");
                _currentController->handleUserInput(PROCESSOR_BUTTON_CANCEL, PROCESSOR_BUTTON_ACTION_CLICK, state);
            }
        }
    }

    // make sure initial drawing is performed
    if (!IS_FLAG_SET(NAVCONTROLLERSTATE_INITIALDRAWDONE, _stateFlags)) {
        if (redrawView()) {
            SET_FLAG(NAVCONTROLLERSTATE_INITIALDRAWDONE, _stateFlags);
        }
    }

    if (IS_FLAG_SET(NAVCONTROLLERSTATE_MODEENTERED, _stateFlags)) {
        // TODO: avoid calls immediately after the controller is activated
        _currentController->handleTick();
    }

    return true;
}

bool MainController::redrawView(){

    auto descriptor = _model.getCurrentNavItemDescriptor();
    if (descriptor != nullptr) {
        _view->drawModeSplash(descriptor);
        return true;
    }
    return false;
}

void MainController::addChildModeController(NavigationTargetDescriptor * controllerInfo, ModeController * controller){
    
    uint8_t modeIndex = _model.addTargetDescriptor(controllerInfo);
    _modeControllers[modeIndex] = controller;
}