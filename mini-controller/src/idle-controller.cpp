#include "idle-controller.h"

IdleController::IdleController(Display * view) :
    _view(view)
{
}

ModeControllerHandleUserInputResult IdleController::handleUserInput(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state) {
    return PROCESSOR_RESULT_SUCCESS;
}