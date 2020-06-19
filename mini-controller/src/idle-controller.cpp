#include "idle-controller.h"

IdleController::IdleController(MainView * view) :
    _view(view)
{
}

ModeControllerHandleUserInputResult IdleController::handleUserInput(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state) {
    return PROCESSOR_RESULT_SUCCESS;
}