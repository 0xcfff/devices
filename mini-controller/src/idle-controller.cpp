#include <macro-flags.h>

#include "xbm/Sleep.xbm"
#include "idle-controller.h"

IdleController::IdleController(Display * view, uint16_t splashDisplayDurationMillis) :
    _view(view),
    _splashDisplayDurationMillis(splashDisplayDurationMillis),
    _state(IDLECONTROLLERSTATE_INACTIVE),
    _stateActivatedAtMillis(0)
{
}

ModeControllerHandleUserInputResultData IdleController::handleUserInput(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state) 
{
    switch (_state)
    {
        case IDLECONTROLLERSTATE_SHOWSLEEPSPLASH:
            return handleUserInputOnSplash(button, action, state);
        case IDLECONTROLLERSTATE_SLEEPING:
            return handleUserInputOnSleep(button, action, state);
        default:
            break;
    }
    return PROCESSOR_RESULT_NONE;
}

ModeControllerHandleUserInputResultData IdleController::handleUserInputOnSplash(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state)
{
    if (action == PROCESSOR_BUTTON_ACTION_CLICK) {
        switch (button)
        {
            case PROCESSOR_BUTTON_CANCEL:
                return PROCESSOR_RESULT_LEAVESTATE;
            default:
                return PROCESSOR_RESULT_SUCCESS;
        }
    }
    return PROCESSOR_RESULT_NONE;
}

ModeControllerHandleUserInputResultData IdleController::handleUserInputOnSleep(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state)
{
    if (action == PROCESSOR_BUTTON_ACTION_CLICK) {
        auto success = deactivateSleepMode();
        if (success) {
            _stateActivatedAtMillis = millis();
            _state = IDLECONTROLLERSTATE_SHOWWAKEUPSPLASH;
            //redrawView();
            return PROCESSOR_RESULT_LEAVESTATE;
        }
    }
    return PROCESSOR_RESULT_NONE;
}

bool IdleController::redrawView(){    
    bool result = true;
    switch (_state)
    {
        case IDLECONTROLLERSTATE_SHOWSLEEPSPLASH:
            _view->drawImageView(Sleep_width, Sleep_height, Sleep_bits, "Баиньки...");
            break;
        case IDLECONTROLLERSTATE_SHOWWAKEUPSPLASH:
            _view->drawImageView(Sleep_width, Sleep_height, Sleep_bits, "С возвращением!");
            break;
        
        default:
            result = false;
    }
    return result;
}


bool IdleController::activate(){
    _state = IDLECONTROLLERSTATE_SHOWSLEEPSPLASH;
    _stateActivatedAtMillis = millis();
    redrawView();
    return true;
}

bool IdleController::deactivate(){
    return true;
}

bool IdleController::handleTick(){
    bool result = true;

    switch (_state)
    {
        case IDLECONTROLLERSTATE_SHOWSLEEPSPLASH: {
            unsigned long waitDuration = millis() - _stateActivatedAtMillis;
            if (waitDuration >= _splashDisplayDurationMillis){
                bool result = activateSleepMode();
                if (result) {
                    _state = IDLECONTROLLERSTATE_SLEEPING;
                }
            }
            break;
        }
        case IDLECONTROLLERSTATE_SHOWWAKEUPSPLASH: {
            unsigned long waitDuration = millis() - _stateActivatedAtMillis;
            if (waitDuration >= _splashDisplayDurationMillis){
                // TODO: initiate leaving of the mode here
            }
            break;
        }
        
        default:
            break;
    }

    return result;
}

bool IdleController::activateSleepMode(){
    return _view->turnOff();
}

bool IdleController::deactivateSleepMode(){
    return _view->turnOn();
}
