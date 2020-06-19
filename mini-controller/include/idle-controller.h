#ifndef _IDLE_CONTROLLER_H_
#define _IDLE_CONTROLLER_H_

#include "display.h"
#include "mode-controller.h"

class IdleController : public ModeController{
    public:
        IdleController(Display * view);

        ModeControllerHandleUserInputResult handleUserInput(ModeControllerCommandButton button, ModeControllerCommandButtonAction action, ModeControllerCommandButton state) override;

    private:
        Display * _view;
};

#endif  // _IDLE_CONTROLLER_H_