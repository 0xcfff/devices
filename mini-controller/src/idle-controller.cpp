#include <macro-flags.h>

#include "xbm/Sleep.xbm"
#include "idle-controller.h"

IdleController::IdleController(Display * view, uint16_t splashDisplayDurationMillis) :
    _view(view),
    _splashDisplayDurationMillis(splashDisplayDurationMillis),
    _stateFlags(IDLECONTROLLERSTATE_EMPTY),
    _activatedAtMillis(0)
{
}

ModeControllerHandleUserInputResult IdleController::handleUserInput(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state) {
    return PROCESSOR_RESULT_UNKNOWN;
}


bool IdleController::activate(){
    _activatedAtMillis = millis();
    _view->drawImageView(Sleep_width, Sleep_height, Sleep_bits, "Баиньки...");
    return true;
}
bool IdleController::deactivate(){
    return true;
}

bool IdleController::handleTick(){
    bool result = true;
    if (!IS_FLAG_SET(IDLECONTROLLERSTATE_SLEEPMODE, _stateFlags)) {
        unsigned long waitDuration = millis() - _activatedAtMillis;
        if (waitDuration >= _splashDisplayDurationMillis){
            bool result = activateSleepMode();
            if (result) {
                SET_FLAG(IDLECONTROLLERSTATE_SLEEPMODE, _stateFlags);
            }
        }
    }
    return result;
}


bool IdleController::activateSleepMode(){
    _view->turnOff();
}

bool IdleController::deactivateSleepMode(){
    _view->turnOn();
}
