#include <macro-logs.h>
#include <macro-flags.h>

#include "navigation-controller.h"

//TODO: move this to proper place
#define NAVCONTROLLER_SLEEP_TIMEOUT     (20 * 1000)

MainController::MainController(Button * modeButton, Button * confirmButton, Button * cancelButton, NavigationView * modeSelectionPresenter, std::vector<std::pair<NavigationTargetDescriptor *, ModeController *>> * childControllers) :
    _modeButton(modeButton),
    _confirmButton(confirmButton),
    _cancelButton(cancelButton),
    _view(modeSelectionPresenter),
    _stateFlags(NAVCONTROLLERSTATE_EMPTY),
    _state(NAVCONTROLLERSTATE_NAVIGATINGTHROUGHCONTROLLERS),
    _currentController(nullptr),
    _lastUserActionMillis(0),
    _sleepMode(false)
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

bool MainController::isSleeping(){
    return _sleepMode;
}

bool MainController::handle(){

    // make sure initial drawing is performed
    if (!IS_FLAG_SET(NAVCONTROLLERSTATE_INITIALDRAWDONE, _stateFlags)) {
        if (redrawView()) {
            SET_FLAG(NAVCONTROLLERSTATE_INITIALDRAWDONE, _stateFlags);
        }
    }

    // actualize input state
    _modeButton->tick();
    _confirmButton->tick();
    _cancelButton->tick();


    bool wasSleeping = _sleepMode;
    processSleepMode();
    bool wokeUp = wasSleeping && !_sleepMode;
    

    // process user input
    if (!wokeUp) {
        HandleInputResult handleInputResult = processUserInput();

        // tick nested controller if needed
        if (_state == NAVCONTROLLERSTATE_ENTEREDNESTEDCONTROLLER && handleInputResult != HANDLEINPUTRESULT_ENTEREDNESTEDCONTROLLER && !_sleepMode) {
            // only call handleTick for nested controller if it was not entered on this cycle
            _currentController->handleTick();
        }
    } 
   
    return true;
}

bool MainController::processSleepMode()
{
    bool result = false;
    bool buttonPressed =_modeButton->isChanged()
        || _confirmButton->isChanged()
        || _cancelButton->isChanged();

    if (_lastUserActionMillis == 0) {
        _lastUserActionMillis = millis();
    }

    if (buttonPressed) {
        if (_sleepMode) 
        {
            ModeController * controllerToSkip = nullptr;
            if (_state == NAVCONTROLLERSTATE_ENTEREDNESTEDCONTROLLER) {
                _currentController->handleSystemEvent(PROCESSOR_EVENT_WAKEUP);
                controllerToSkip = _currentController;
            }
            for(int i = 0; i < (int)_modeControllers.size(); i++) {
                auto controller = _modeControllers.at(i);
                if (controller != controllerToSkip) {
                    controller->handleSystemEvent(PROCESSOR_EVENT_WAKEUP);
                }
            }
            _sleepMode = false;
            result = true;
        }
        _lastUserActionMillis = millis();
    } else {
        if (!_sleepMode) {
            if ((millis() - _lastUserActionMillis) > NAVCONTROLLER_SLEEP_TIMEOUT) 
            {
                bool postpone = false;
                ModeController * controllerToSkip = nullptr;
                if (_state == NAVCONTROLLERSTATE_ENTEREDNESTEDCONTROLLER) {
                    auto result = _currentController->handleSystemEvent(PROCESSOR_EVENT_SLEEP);
                    if (result == PROCESSOR_EVENTRESULT_POSTPONE) {
                        postpone = true;
                    } else {
                        controllerToSkip = _currentController;
                    }
                }
                if (postpone) {
                    _lastUserActionMillis = millis();
                } else {
                    for(int i = 0; i < (int)_modeControllers.size(); i++) {
                        auto controller = _modeControllers.at(i);
                        if (controller != controllerToSkip) {
                            controller->handleSystemEvent(PROCESSOR_EVENT_SLEEP);
                        }
                    }
                    _sleepMode = true;
                    result = true;
                }
            }
        }
    }
    return result;
}

MainController::HandleInputResult MainController::processUserInput()
{
    ModeControllerCommandButton state = (ModeControllerCommandButton)(PROCESSOR_BUTTON_NONE 
        | (_modeButton->isPressed() ? PROCESSOR_BUTTON_MENU : 0) 
        | (_confirmButton->isPressed() ? PROCESSOR_BUTTON_OK : 0)
        | (_cancelButton->isPressed() ? PROCESSOR_BUTTON_CANCEL : 0)
        );

    HandleInputResult handleInputResult = HANDLEINPUTRESULT_NONE;
    
    if (_modeButton->isChanged()) {
        handleInputResult = handleUserInput(PROCESSOR_BUTTON_MENU, translateButtonActionToMCButtonAction(_modeButton->getButtonAction()), state);
    }
    if (_confirmButton->isChanged()) {
        handleInputResult = handleUserInput(PROCESSOR_BUTTON_OK, translateButtonActionToMCButtonAction(_confirmButton->getButtonAction()), state);
    }
    if (_cancelButton->isChanged()) {
        handleInputResult = handleUserInput(PROCESSOR_BUTTON_CANCEL, translateButtonActionToMCButtonAction(_cancelButton->getButtonAction()), state);
    }

    return handleInputResult;
}

MainController::HandleInputResult MainController::handleUserInput(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state)
{
    HandleInputResult result = HANDLEINPUTRESULT_NONE;
    bool dispatched = false;
    switch (_state)
    {
        case NAVCONTROLLERSTATE_NAVIGATINGTHROUGHCONTROLLERS:
            result = handleUserInputWhenNavigate(button, action, state);
            dispatched = true;
            break;
        case NAVCONTROLLERSTATE_ENTEREDNESTEDCONTROLLER:
            result = handleUserInputWhenEnteredNestedController(button, action, state);
            dispatched = true;
        default:
            break;
    }

    if (dispatched && result == HANDLEINPUTRESULT_NONE) {
        // TODO: dispatch to other controllers
    }
    return result;
}

MainController::HandleInputResult MainController::handleUserInputWhenNavigate(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state)
{    
    if (action == PROCESSOR_BUTTON_ACTION_CLICK){
        switch (button)
        {
            case PROCESSOR_BUTTON_MENU:
                LOG_DEBUGLN("Mode button click detected!");
                if (_model.navigateNextNavItem()) {
                    redrawView();
                }
                return HANDLEINPUTRESULT_HANDLEDBYSELF;
            case PROCESSOR_BUTTON_OK: {
                LOG_DEBUGLN("Confirm button click detected!");
                auto controllerIndex = _model.getCurrentNavItemIndex();
                auto controller = _modeControllers.at(controllerIndex);
                if (controller->activate()) {
                    _currentController = controller;
                    _state = NAVCONTROLLERSTATE_ENTEREDNESTEDCONTROLLER;
                }
                return HANDLEINPUTRESULT_ENTEREDNESTEDCONTROLLER;
            }
            default:
                // any button clicked once in navigation mode is considered as handled by navigation controller
                return HANDLEINPUTRESULT_HANDLEDBYSELF;
        }
    }

    return HANDLEINPUTRESULT_NONE;
}

MainController::HandleInputResult MainController::handleUserInputWhenEnteredNestedController(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state)
{
    auto result = _currentController->handleUserInput(button, action, state);
    switch (result.handleResult)
    {
        case PROCESSOR_RESULT_NONE:
            return HANDLEINPUTRESULT_NONE;
        case PROCESSOR_RESULT_SUCCESS:
        case PROCESSOR_RESULT_ERROR:
            return HANDLEINPUTRESULT_HANDLEDBYNESTEDCONTROLLER;
        case PROCESSOR_RESULT_LEAVESTATE: {
            LOG_DEBUGLN("Leave state requested!");
            _currentController->deactivate();
            _state = NAVCONTROLLERSTATE_NAVIGATINGTHROUGHCONTROLLERS;
            redrawView();
            return HANDLEINPUTRESULT_LEFTNESTEDCONTROLLER;
        }
        case PROCESSOR_RESULT_RISEEVENT: {
            // TODO: implement rising events functionality, below result value is wrong
            return HANDLEINPUTRESULT_HANDLEDBYNESTEDCONTROLLER;
        }
        default:
            return HANDLEINPUTRESULT_NONE;
    }
    return HANDLEINPUTRESULT_NONE;
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

ModeControllerCommandButtonAction MainController::translateButtonActionToMCButtonAction(ButtonAction action){
    switch (action)
    {
        case BUTTON_ACTION_NONE:
            return PROCESSOR_BUTTON_ACTION_NONE;
        case BUTTON_ACTION_CLICK:
            return PROCESSOR_BUTTON_ACTION_CLICK;
        case BUTTON_ACTION_DBLCLICK:
            return PROCESSOR_BUTTON_ACTION_DBLCLICK;
        case BUTTON_ACTION_LONGPRESS:
            return PROCESSOR_BUTTON_ACTION_LONGPRESS;
        default:
            return PROCESSOR_BUTTON_ACTION_NONE;
    }
    return PROCESSOR_BUTTON_ACTION_NONE;
}